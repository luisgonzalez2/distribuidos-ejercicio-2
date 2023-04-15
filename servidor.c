#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "lines.h"
#include "mensaje.h"
#include "tratamiento_serv.h"
#include "separar_mensaje.h"

#define MAX_THREADS 10
#define MAX_PETICIONES 256
#define SERVIDOR "/SERVIDOR" // Pasar como var de entorno

pthread_mutex_t m_params;
pthread_attr_t t_attr;
pthread_cond_t cond;
int listo;
int sd, sc; // Descriptores de fichero

int servicio(void *args)
{
	char mensaje[1024];
	int sc_local;
	int err = 0;

	printf("En el hilo\n");

	pthread_mutex_lock(&m_params);
	sc_local = (*(int *)args);
	printf("Socket:%d\n", sc_local);
	listo = 1;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&m_params);

	// Con la petición recibida, mandan al hilo esa petición como CHAR
	pthread_mutex_lock(&m_params);
	err = readLine(sc_local, mensaje, 1024);

	if (err == -1)
	{
		perror("Error en recepcion1\n");
		close(sc);
		exit(-1);
	}

	struct respuesta res = tratar_peticion(mensaje);
	char *msg_respuesta;
	msg_respuesta = respuesta_to_char(res);

	// Mandar al socket del cliente
	err = sendMessage(sc_local, (char *)msg_respuesta, strlen(msg_respuesta) + 1);
	if (err == -1)
	{
		perror("Error en envio de mensaje\n");
		close(sc_local);
		free(msg_respuesta);
		pthread_exit(NULL);
	}

	pthread_mutex_unlock(&m_params);

	free(msg_respuesta);
	close(sc_local);
	pthread_exit(NULL);
}

int main(void)
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t size;
	int val, err;

	// 1. Abre socket del servidor
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("SERVER: Error en el socket");
		return (0);
	}

	val = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// Puerto variable de entorno
	int port_tuplas;
	if (getenv("PORT_TUPLAS") == NULL)
	{
		printf("Variable VAR1 no definida\n");
		return 0;
	}
	else
	{
		port_tuplas = atoi(getenv("PORT_TUPLAS"));
	}
	server_addr.sin_port = htons(port_tuplas);

	err = bind(sd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		perror("Error en bind\n");
		return -1;
	}

	err = listen(sd, SOMAXCONN);
	if (err == -1)
	{
		perror("Error en listen\n");
		return -1;
	}

	size = sizeof(client_addr);

	// 2. Crea hilos bajo demanda a los que envía las peticiones recibidas
	pthread_attr_init(&t_attr);
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

	while (1)
	{
		pthread_t hilo;
		// Recibe el mensaje mandado por claves.c en su cola servidor
		printf("Esperando conexion...\n");
		// Acepta conexión
		sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
		if (sc == -1)
		{
			perror("Error en accept\n");
			continue; // Volver a intentar aceptar conexiones
		}
		printf("Conexión aceptada de IP: %s Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		// Recibe mensaje
		// err = recvMessage(sc, (char *)&mess, sizeof(char)); // recibe la operación

		listo = 0;
		if (pthread_create(&hilo, NULL, (void *)servicio, &sc) != 0)
		{
			perror("Error creando thread\n");
			close(sc); // Cerrar conexión
			continue;  // Volver a intentar aceptar conexiones
		}

		// Hasta que el hilo creado no tome la petición, no se crean más hilos
		pthread_mutex_lock(&m_params);
		while (!listo)
		{
			pthread_cond_wait(&cond, &m_params);
		}
		pthread_mutex_unlock(&m_params);
	}
	return 0;
}
// PORT_TUPLAS=8080 ./servidor