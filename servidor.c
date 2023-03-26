#include <stdio.h>
#include<stdlib.h>
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
	pthread_mutex_lock(&m_params);
	memcpy(&mensaje, args, 1024);
	listo = 1;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&m_params);

	struct respuesta res = tratar_peticion(mensaje);
	char msg_respuesta[1024];
	strcpy(msg_respuesta,respuesta_to_char(res));
	// Mandar al socket del cliente
	sendMessage(sc, msg_respuesta, sizeof(msg_respuesta));
	pthread_exit(NULL);
}

int main(void)
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t size;
	int val;
	int err;


	// 1. Abre socket del servidor
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("SERVER: Error en el socket");
		return (0);
	}
	val = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

	printf("Abre cola de servidor\n");
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	char ip_tuplas[16];
	// IP de la variable de entorno
	strcpy(ip_tuplas, getenv("IP_TUPLAS"));
	server_addr.sin_addr.s_addr = inet_addr(ip_tuplas);
	// Puerto variable de entorno
	int port_tuplas;
	port_tuplas = atoi(getenv("PORT_TUPLAS"));
	server_addr.sin_port = htons(port_tuplas);

	err = bind(sd, (const struct sockaddr *)&server_addr,
			   sizeof(server_addr));
	if (err == -1)
	{
		printf("Error en bind\n");
		return -1;
	}

	err = listen(sd, SOMAXCONN);
	if (err == -1)
	{
		printf("Error en listen\n");
		return -1;
	}

	size = sizeof(client_addr);

	// 2. Crea hilos bajo demanda a los que envía las peticiones recibidas
	pthread_attr_init(&t_attr);

	while (1)
	{
		struct peticion mess; // Mensaje a recibir
		pthread_t hilo;
		// Recibe el mensaje mandado por claves.c en su cola servidor
		printf("esperando conexion\n");
		//----Acepta conexión
		sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
		if (sc == -1)
		{
			printf("Error en accept\n");
			return -1;
		}
		printf("Conexión aceptada de IP: %s Puerto: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		// Recibe mensaje
		err = recvMessage(sc, (char *)&mess, sizeof(char)); // recibe la operación
		if (err == -1)
		{
			printf("Error en recepcion\n");
			close(sc);
			continue;
		}
		// Con la petición recibida, mandan al hilo esa petición como CHAR
		listo = 0;
		if (pthread_create(&hilo, NULL, (void *)servicio, &mess) != 0)
		{
			perror("Error creando thread\n");
			return 0;
		}
		// Hasta que el hilo creado no tome la petición, no se crean más hilos
		pthread_mutex_lock(&m_params);
		while (!listo)
		{
			pthread_cond_wait(&cond, &m_params);
		}
		pthread_mutex_unlock(&m_params);
		pthread_join(hilo, NULL);
	}

	return 0;
}

//env IP_TUPLAS=127.0.0.1 PORT_TUPLAS=8080 ./servidor

