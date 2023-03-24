#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include "mensaje.h"
#include "tratamiento_serv.h"

#define MAX_THREADS 10
#define MAX_PETICIONES 256
#define SERVIDOR "/SERVIDOR"

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
	pthread_exit(NULL);
}

int main(void)
{
	mqd_t q_servidor;	   // Cola de mensajes del proceso servidor
	struct mq_attr q_attr; // Atributos de la cola de servidor
	pthread_attr_t t_attr;
	q_attr.mq_maxmsg = 5;
	q_attr.mq_msgsize = sizeof(struct peticion);

	int error;

	// 1. Abre cola del servidor
	q_servidor = mq_open(SERVIDOR, O_CREAT | O_RDONLY, 0700, &q_attr);
	if (q_servidor == -1)
	{
		perror("mq_open");
		return -1;
	}
	printf("Abre cola de servidor\n");

	// 2. Crea hilos bajo demanda a los que envía las peticiones recibidas
	pthread_attr_init(&t_attr);
	while (true)
	{
		struct peticion mess; // Mensaje a recibir
		pthread_t hilo;
		// Recibe el mensaje mandado por claves.c en su cola servidor
		error = mq_receive(q_servidor, (char *)&mess, sizeof(struct peticion), 0);
		if (error == -1)
		{
			perror("mq_receive");
			break;
		}
		// Con la petición recibida, mandan al hilo esa petición
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
