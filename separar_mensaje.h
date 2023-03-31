#ifndef _SEPARAR_MENSAJE_H
#define _SEPARAR_MENSAJE_H
void separarMensajePeticion(char mensaje[1024], int *op, int *clave, char v1[256], int *v2, double *v3, char q_name[256], int *clave2);
struct peticion char_to_peticion(char mensaje[1024]);
void separarMensajeRespuesta(char mensaje[1024], int *respuesta, char v1[256], int *v2, double *v3);
struct respuesta char_to_respuesta(char mensaje[1024]);
char *peticion_to_char(struct peticion p);
char *respuesta_to_char(struct respuesta r);
#endif
