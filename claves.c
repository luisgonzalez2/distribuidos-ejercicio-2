#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "mensaje.h"		 // Struct 'peticion' y 'respuesta'
#include "lines.h"			 // Funciones 'sendMessage' y 'readLine'
#include "separar_mensaje.h" // Función 'char_to_respuesta'

struct respuesta mandar_servidor(struct peticion pet)
{
	int sd, err;
	struct respuesta res;
	char respuesta[1024];
	char *mensaje;
	struct hostent *hp = NULL;
	struct sockaddr_in server_addr = {0};

	// 1. Abre socket del usuario
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1)
	{
		perror("Error al crear el socket\n");
		res.respuesta = -1;
		return res;
	}

	// 2. Abrir socket del servidor
	char *ip_tuplas = NULL;
	if (getenv("IP_TUPLAS") == NULL)
	{
		printf("Variable VAR1 no definida\n");
		res.respuesta = -1;
		return res;
	}
	else
	{
		ip_tuplas = getenv("IP_TUPLAS");
	}
	int port_tuplas;
	if (getenv("PORT_TUPLAS") == NULL)
	{
		printf("Variable VAR1 no definida\n");
		res.respuesta = -1;
		return res;
	}
	else
	{
		port_tuplas = atoi(getenv("PORT_TUPLAS"));
	}
	bzero((char *)&server_addr, sizeof(server_addr));
	hp = gethostbyname(ip_tuplas);
	if (hp == NULL)
	{
		perror("Error al obtener el host\n");
		close(sd);
		res.respuesta = -1;
		return res;
	}

	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_tuplas);

	// 3. Establece la conexión
	err = connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		perror("Error al establecer la conexión\n");
		close(sd);
		res.respuesta = -1;
		return res;
	}

	// 4. Pasar struct petición a char
	mensaje = peticion_to_char(pet);

	// 5. Mandar petición
	err = sendMessage(sd, (char *)mensaje, strlen(mensaje) + 1); // Envía petición
	if (err == -1)
	{
		perror("Error al enviar el mensaje\n");
		close(sd);
		free(mensaje);
		res.respuesta = -1;
		return res;
	}

	// 6. Recibir respuesta
	err = readLine(sd, respuesta, 1024);
	if (err == -1)
	{
		perror("Error al recibir la respuesta\n");
		close(sd);
		free(mensaje);
		res.respuesta = -1;
		return res;
	}

	// 7. Pasar de char a un struct respuesta
	res = char_to_respuesta(respuesta);

	//  8. Cerramos colas
	close(sd);
	free(mensaje);
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