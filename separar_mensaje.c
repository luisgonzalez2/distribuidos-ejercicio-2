#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mensaje.h"

void separarMensajePeticion(char *mensaje, int *op, int *clave, char *v1, int *v2, float *v3, char *q_name, int *clave2)
{
    char *token;
    token = strtok(mensaje, ";");
    *op = atoi(token);
    token = strtok(mensaje, ";");
    *clave = atoi(token);
    token = strtok(NULL, ";");
    strcpy(v1, token);
    token = strtok(NULL, ";");
    *v2 = atoi(token);
    token = strtok(NULL, ";");
    *v3 = atof(token);
    token = strtok(NULL, ";");
    strcopy(q_name, token);
    token = strtok(mensaje, ";");
    *clave2 = atoi(token);
}

struct peticion char_to_peticion (mensaje)
{
    // separarMensajePeticion separa el char en los elementos del struct
    char v1, q_name;
    int op, clave, v1, v2, clave2;
    float v3;
    separarMensajePeticion(mensaje, &op, &clave, v1, &v2, &v3, q_name, &clave2);

    // Añadimos los valores del mensaje al struct
    struct peticion pet;
    pet.op = op;
    pet.clave = clave;
    strcopy(pet.valor1, v1);
    pet.valor2 = v2;
    pet.valor3 = v3;
    strcopy(pet.q_name, q_name);
    pet.clave2 = clave2;

    return pet;
}

void separarMensajeRespuesta(char *mensaje, int *respuesta, char *v1, int *v2, float *v3)
{
    char *token;
    token = strtok(mensaje, ";");
    *respuesta = atoi(token);
    token = strtok(mensaje, ";");
    strcpy(v1, token);
    token = strtok(NULL, ";");
    *v2 = atoi(token);
    token = strtok(NULL, ";");
    *v3 = atof(token);
}

struct respuesta char_to_respuesta (mensaje)
{
    // separarMensajeRespuesta separa el char en los elementos del struct
    char v1;
    int respuesta, v1, v2;
    float v3;
    separarMensajeRespuesta(mensaje, &respuesta, v1, &v2, &v3);

    // Añadimos los valores del mensaje al struct
    struct respuesta res;
    res.respuesta = respuesta;
    strcopy(res.valor1, v1);
    res.valor2 = v2;
    res.valor3 = v3;

    return res;
}
