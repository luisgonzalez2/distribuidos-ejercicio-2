#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>
#include "mensaje.h"

// Función auxiliar que separa los elementos de la cadena de caracteres en variables
void separarMensajePeticion(char *mensaje, int *op, int *clave, char v1[256], int *v2, double *v3, char q_name[256], int *clave2)
{
    char *token;
    token = strtok(mensaje, ";");
    if (token != NULL)
    {
        *op = atoi(token);
    }
    else
    {
        *op = 0;
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *clave = atoi(token);
    }
    else
    {
        *clave = 0;
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        strcpy(v1, token);
    }
    else
    {
        strcpy(v1, "");
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *v2 = atoi(token);
    }
    else
    {
        *v2 = 0;
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *v3 = atof(token);
    }
    else
    {
        *v3 = 0.0;
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *clave2 = atoi(token);
    }
    else
    {
        *clave2 = 0;
    }
}

// Transforma la cadena de caracteres en estructuras de tipo petición que usamos en el servidor
struct peticion char_to_peticion(char *mensaje)
{
    char v1[256], q_name[256];
    int op, clave, v2, clave2;
    double v3;
    separarMensajePeticion(mensaje, &op, &clave, v1, &v2, &v3, q_name, &clave2);

    // Añadimos los valores del mensaje al struct
    struct peticion pet;
    pet.op = op;
    pet.clave = clave;
    strcpy(pet.valor1, v1);
    pet.valor2 = v2;
    pet.valor3 = v3;
    pet.clave2 = clave2;

    return pet;
}

// Función auxiliar que separa los elementos de la cadena de caracteres en variables
void separarMensajeRespuesta(char mensaje[1024], int *respuesta, char v1[256], int *v2, double *v3)
{
    char *token;
    token = strtok(mensaje, ";");
    *respuesta = atoi(token);
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        strcpy(v1, token);
    }
    else
    {
        strcpy(v1, "");
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *v2 = atoi(token);
    }
    else
    {
        *v2 = 0;
    }
    token = strtok(NULL, ";");
    if (token != NULL)
    {
        *v3 = atof(token);
    }
    else
    {
        *v3 = 0.0;
    }
}

// Transforma la cadena de caracteres en estructuras de tipo respuesta que usamos en el servidor
struct respuesta char_to_respuesta(char *mensaje)
{
    char v1[256];
    int respuesta, v2;
    double v3;
    separarMensajeRespuesta(mensaje, &respuesta, v1, &v2, &v3);

    // Añadimos los valores del mensaje al struct
    struct respuesta res;
    res.respuesta = respuesta;
    strcpy(res.valor1, v1);
    res.valor2 = v2;
    res.valor3 = v3;

    return res;
}

// Transforma las estructuras de tipo petición en cadenas de caracteres que se pueden trasmitir por los sockets
char *peticion_to_char(struct peticion p)
{
    char *resultado = malloc(1024);
    sprintf(resultado, "%d;%d;%s;%d;%.2f;%d;", p.op, p.clave, p.valor1, p.valor2, p.valor3, p.clave2);
    return resultado;
}

// Transforma las estructuras de tipo respuesta en cadenas de caracteres que se pueden trasmitir por los sockets
char *respuesta_to_char(struct respuesta r)
{
    char *resultado = malloc(1024);
    sprintf(resultado, "%d;%s;%d;%.2f;", r.respuesta, r.valor1, r.valor2, r.valor3);
    return resultado;
}
