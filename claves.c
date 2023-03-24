#include <stdio.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "mensaje.h"
#define SERVIDOR "/SERVIDOR"

struct respuesta mandar_servidor(struct peticion pet)
{	int sd;
    struct sockaddr_in server_addr;//¿Cual es el tamaño del server? a lo mejor hay que editarlo
    struct hostent *hp;
    int32_t a, b, res;          
	char op;
	int err;
	struct respuesta res;
	char respuesta;

	// 1. Abre socket del usuario
	char queuename[MAXSIZE];
	printf("Abre cola de usuario\n");
	sprintf(queuename, "/Cola-%d", getpid());
	strcpy(pet.q_name, queuename);
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		// return -1; // función de tipo struct
	}

	// 2. Abrir socket del servidor 
	bzero((char *)&server_addr, sizeof(server_addr));
   	hp = gethostbyname (SERVIDOR);//El nombre del servidor es el establecido
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		// return -1; // función de tipo struct
	};
   	memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
   	server_addr.sin_port    = htons(4200);
	//3.Establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		// return -1; // función de tipo struct
	}//4.Pasar struct petición a char
	//-----------------------------------
	// 5.Mandar petición
	err = sendMessage(sd, (char *) &op, sizeof(char));  // envía la operacion
	if (err == -1){
		printf("Error en envio\n");
		return -1;
	};
	printf("Mensaje mandado\n");
	//6.Recibir respuesta
	err = recvMessage(sd, (char *) &respuesta, sizeof(int32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
		return -1;//Mirar esto
	};
	printf("respuesta llegó a su destino\n");
	//7.Pasar de char a un struct respuesta
	//------------------------------
	// 8. Cerramos colas
	close (sd);
	return res.respuesta;
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
