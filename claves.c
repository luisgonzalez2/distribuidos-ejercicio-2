#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"
#define SERVIDOR "/SERVIDOR"

struct respuesta mandar_servidor(struct peticion pet)
{
	mqd_t q_servidor;			// Cola de mensajes del proceso servidor
	mqd_t q_cliente;			// Cola de mensajes para el proceso cliente
	struct mq_attr client_attr; // Atributos de la cola de cliente
	client_attr.mq_msgsize = sizeof(struct respuesta);
	client_attr.mq_maxmsg = 5;
	struct respuesta res;

	// 1. Abre cola del usuario
	char queuename[MAXSIZE];
	printf("Abre cola de usuario\n");
	sprintf(queuename, "/Cola-%d", getpid());
	strcpy(pet.q_name, queuename);
	q_cliente = mq_open(queuename, O_CREAT | O_RDONLY, 0700, &client_attr);
	if (q_cliente == -1)
	{
		perror("mq_open 1");
		res.respuesta = -1;
		return res;
	}
	struct mq_attr serv_attr;
	serv_attr.mq_msgsize = sizeof(struct peticion);
	serv_attr.mq_maxmsg = 5;

	// 2. Manda al servidor la petición
	q_servidor = mq_open(SERVIDOR, O_CREAT | O_WRONLY, 0700, &serv_attr);
	if (q_servidor == -1)
	{
		mq_close(q_cliente);
		perror("mq_open 2");
		res.respuesta = -1;
		return res;
	}
	if (mq_send(q_servidor, (const char *)&pet, sizeof(pet), 0) < 0)
	{
		perror("mq_send");
		res.respuesta = -1;
		return res;
	}
	mq_getattr(q_servidor, &serv_attr);
	printf("Msg servidor:%ld\n", serv_attr.mq_curmsgs);

	// 3. Recibe la respuesta
	printf("Mensaje mandado\n");
	mq_getattr(q_cliente, &client_attr);
	printf("Msg receptor:%ld\n", client_attr.mq_curmsgs);
	if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0)
	{
		perror("mq_recv 1");
		res.respuesta = -1;
	}
	printf("respuesta llegó a su destino\n");

	// 4. Cerramos colas
	mq_close(q_servidor);
	mq_close(q_cliente);
	mq_unlink(queuename);
	return res;
}

int init()
{
	struct peticion pet;
	pet.op = 0; // Operación de la petición: init
	return mandar_servidor(pet).respuesta;
}

int set_value(int clave, char valor1[256], int valor2, double valor3)
{
	struct peticion pet;
	pet.op = 2; // Operación de la petición: set_value

	pet.clave = clave;
	strcpy(pet.valor1, valor1);
	pet.valor2 = valor2;
	pet.valor3 = valor3;

	return mandar_servidor(pet).respuesta;
}

int get_value(int clave, char *valor1, int *valor2, double *valor3)
{
	struct peticion pet;
	struct respuesta respuesta;
	pet.op = 1; // Operación de la petición: get_value

	pet.clave = clave;
	respuesta = mandar_servidor(pet);

	strcpy(valor1, respuesta.valor1);
	*valor2 = respuesta.valor2;
	*valor3 = respuesta.valor3;

	return respuesta.respuesta;
}

int modify_value(int clave, char valor1[256], int valor2, double valor3)
{
	struct peticion pet;
	pet.op = 3; // Operación de la petición: modify_value

	pet.clave = clave;
	strcpy(pet.valor1, valor1);
	pet.valor2 = valor2;
	pet.valor3 = valor3;

	return mandar_servidor(pet).respuesta;
}

int exist(int clave)
{
	struct peticion pet;
	pet.op = 4; // Operación de la petición: exist

	pet.clave = clave;

	return mandar_servidor(pet).respuesta;
}

int delete_key(int clave)
{
	struct peticion pet;
	pet.op = 5; // Operación de la petición: delete_key

	pet.clave = clave;

	return mandar_servidor(pet).respuesta;
}

int copy_key(int clave, int clave2)
{
	struct peticion pet;
	pet.op = 6; // Operación de la petición: copy_key

	pet.clave = clave;
	pet.clave2 = clave2;

	return mandar_servidor(pet).respuesta;
}
