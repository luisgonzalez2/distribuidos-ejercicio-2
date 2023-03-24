#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "lines.h"
#include "mensaje.h"
#include "tratamiento_serv.h"

#define MAX_THREADS 10
#define MAX_PETICIONES 256
#define SERVIDOR "/SERVIDOR"//Pasar como var de entorno

mqd_t q_servidor;
pthread_mutex_t m_params;
pthread_cond_t cond;
int listo;

int servicio(void *args)
{
	struct peticion mensaje;

	pthread_mutex_lock(&m_params);
	memcpy(&mensaje, args, sizeof(struct peticion));
	listo = 1;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&m_params);

	tratar_peticion(mensaje);
	//Los resultados pasarlos a ntols
	pthread_exit(NULL);
}

int main(void)
{
	struct sockaddr_in server_addr,  client_addr;
	socklen_t size;
    int sd, sc;
    int val;
    char op;
    int32_t a, b,  res;
	int err;
	char mess;
	// 1. Abre socket del servidor
	if ((sd =  socket(AF_INET, SOCK_STREAM, 0))<0){
                printf ("SERVER: Error en el socket");
                return (0);
    }
    val = 1;//Mirar esto
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	printf("Abre cola de servidor\n");
	bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(4200);

    err = bind(sd, (const struct sockaddr *)&server_addr,
		sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    	err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

    size = sizeof(client_addr);

	// 2. Crea hilos bajo demanda a los que envía las peticiones recibidas
	pthread_attr_init(&t_attr);
	while (true)
	{
		struct peticion mess; // Mensaje a recibir
		pthread_t hilo;
		// Recibe el mensaje mandado por claves.c en su cola servidor
		printf("esperando conexion\n");
		//----Acepta conexión
    	sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
		if (sc == -1) {
			printf("Error en accept\n");
			return -1;
		}
		printf("conexiÃ³n aceptada de IP: %s   Puerto: %d\n",
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		//Recive mensaje
  		err = recvMessage ( sc, (char *) &mess, sizeof(char));   // recibe la operaciÃ³
		if (err == -1) {
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
