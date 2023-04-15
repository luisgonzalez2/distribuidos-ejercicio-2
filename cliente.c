#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // PRUEBA 1:INIT -------------------------------------------------------------------------------
    if (init() != 0)
    { // Maneja cualquier error que se produzca al inicializar el cliente
        printf("Error en init\n");
        return -1;
    }

    // PRUEBA 2:SET_VALUE DE UN VALOR QUE NO EXISTE--------------------------------------------------------------------------
    int clave = 1;
    char valor1[256] = "Hola";
    int valor2 = 4;
    double valor3 = 1.2;
    printf("Set\n");
    if (set_value(clave, valor1, valor2, valor3) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en set\n");
        return -1;
    }
    // PRUEBA 3:GET_VALUE (EXISTE EL VALOR) --------------------------------------------------------------------------
    char valor1g[256];
    int valor2g;
    double valor3g;
    printf("Get\n");
    if (get_value(clave, valor1g, &valor2g, &valor3g) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        perror("Error en get\n");
        return -1;
    }
    printf("Valor1:%s\n", valor1g);
    printf("Valor2:%d\n", valor2g);
    printf("Valor3:%lf\n", valor3g);

    // PRUEBA 4:EXIST(EXISTE EL VALOR) ------------------------------------------------------------------------------
    printf("Exist\n");
    int respuesta = exist(clave);
    if (respuesta < 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        perror("Error en exist\n");
        return -1;
    }
    else if (respuesta == 0)
    {
        printf("No existe la clave\n");
    }
    else
    {
        printf("Existe la clave\n");
    }
    //PRUEBA 2.1:SET_VALUE DE UNA CLAVE EXISTENTE--------------------------------------------------------------------------
    int clave_2 = 1;
    char valor1_2[256] = "Chocolate";
    int valor2_2 = 22;
    double valor3_2 = 1.7;
    printf("Set\n");
    if (set_value(clave_2, valor1_2, valor2_2, valor3_2) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en set\n");
        return -1;
    }
}
// IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente