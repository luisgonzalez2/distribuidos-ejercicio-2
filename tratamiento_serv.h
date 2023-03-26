#ifndef _TRATA_H
#define _TRATA_H

int exist(int clave);
char *buscar_clave(int clave);
int delete_key(int clave);
int eliminar_tuplas();
int init();
int set_value(int clave, char valor1[256], int valor2, double valor3, char q_name[256]);
struct respuesta get_value(int clave, char *valor1, int *valor2, double *valor3);
int modify_value(int clave, char valor1[256], int valor2, double valor3, char q_name[256]);
int copy_key(int key1, int key2);
struct respuesta tratar_peticion(char mensaje[1024]);
#endif
