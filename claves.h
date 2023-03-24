#ifndef _CLAVES_H
#define _CLAVES_H
#include "mensaje.h"
struct respuesta mandar_servidor(struct peticion pet);
int init();
int set_value(int clave, char valor1[256], int valor2, double valor3);
int get_value(int clave, char *valor1, int *valor2, double *valor3);
int modify_value(int clave, char valor1[256], int valor2, double valor3);
int exist(int clave);
int delete_key(int clave);
int copy_key(int clave, int clave2);
#endif
