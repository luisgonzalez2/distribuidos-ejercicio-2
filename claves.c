#include <stdio.h>
#include <stdlib.h> 
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "mensaje.h"
#include "lines.h"
#include "separar_mensaje.h"
#define SERVIDOR "/SERVIDOR"

struct respuesta mandar_servidor(struct peticion pet)
{
	int sd;
	struct sockaddr_in server_addr;
	struct hostent *hp;
	int err;
	struct respuesta res;
	char respuesta[1024];
	char mensaje[1024];

	// 1. Abre socket del usuario
	char queuename[MAXSIZE];
	printf("Abre cola de usuario\n");
	sprintf(queuename, "/Cola-%d", getpid());
	strcpy(pet.q_name, queuename);
	
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1)
	{
		printf("Error en socket\n");
		res.respuesta = -1;
		return res;
	}

	// 2. Abrir socket del servidor
	char *ip_tuplas;
	ip_tuplas = getenv("IP_TUPLAS");
	struct in_addr addr;
	inet_aton(ip_tuplas, &addr);
///------------------------>PRUEBA
	if (inet_aton(ip_tuplas, &addr) == 0)
	{
		printf("Error en inet_aton\n");
		res.respuesta = -1;
		return res;
	}

	bzero((char *)&server_addr, sizeof(server_addr));
	hp = gethostbyaddr(&addr, sizeof(addr), AF_INET);
	//he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
	if (hp == NULL)
	{
		printf("Error en gethostbyaddr\n");
		res.respuesta = -1;
		return res;
	}
	char *port_tuplas;  // Declarar port_tuplas como un puntero a char
	port_tuplas = getenv("PORT_TUPLAS");  // Asignar el valor de la variable de entorno
	memcpy(&(server_addr.sin_addr), hp->h_addr_list, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port_tuplas));
	memset(&server_addr, 0, sizeof(server_addr));
	printf("Conoce al servidor\n");


	// 3.Establece la conexión
	err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		printf("Error en connect\n");
		res.respuesta = -1;
		return res;
	}
	printf("Conecta con el servidor\n");
	// 4.Pasar struct petición a char
	strcpy(mensaje, peticion_to_char(pet));

	// 5.Mandar petición
	err = sendMessage(sd, (char *)&mensaje, sizeof(mensaje)); // Envía petición
	if (err == -1)
	{
		printf("Error en envio\n");
		res.respuesta = -1;
		return res;
	}
	printf("Mensaje mandado\n");

	// 6.Recibir respuesta
	err = recvMessage(sd, (char *)&respuesta, sizeof(respuesta)); // Recibe respuesta
	if (err == -1)
	{
		printf("Error en recepcion\n");
		res.respuesta = -1;
		return res;
	}
	printf("Respuesta llegó a su destino\n");

	// 7.Pasar de char a un struct respuesta
	res = char_to_respuesta(respuesta);

	//  8. Cerramos colas
	close(sd);
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
