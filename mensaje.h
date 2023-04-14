#define MAXSIZE 256
#define MAX_VALOR1 256 // Cadenas de, como mucho, 256 caracteres

struct peticion
{
	int op;
	int clave;
	char valor1[MAX_VALOR1];
	int valor2;
	double valor3;
	char q_name[MAXSIZE]; // Nombre de la cola cliente donde debe enviar la respuesta el servidor
	int clave2;
};

struct respuesta
{
	int respuesta;
	char valor1[MAX_VALOR1];
	int valor2;
	double valor3;
};