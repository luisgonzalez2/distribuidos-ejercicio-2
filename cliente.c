#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // INIT -------------------------------------------------------------------------------
    if (init() != 0)
    { // Maneja cualquier error que se produzca al inicializar el cliente
        printf("Error en init\n");
        return -1;
    }

    // SET_VALUE --------------------------------------------------------------------------
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

    // GET_VALUE --------------------------------------------------------------------------
    char valor1g[256];
    int valor2g;
    double valor3g;
    printf("Get\n");
    if (get_value(clave, valor1g, &valor2g, &valor3g) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en get\n");
        return -1;
    }

    printf("Valor2 devuelto: %d\n", valor2);

    // MODIFY_VALUE -----------------------------------------------------------------------
    char valor1m[256] = "Leche";
    int valor2m = 8;
    double valor3m = 4.5;
    printf("Modify\n");
    if (modify_value(clave, valor1m, valor2m, valor3m) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en modify\n");
        return -1;
    }

    // EXIST ------------------------------------------------------------------------------
    printf("Exist\n");
    if (exist(clave) < 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en exist\n");
        return -1;
    }
    // COPY_KEY ---------------------------------------------------------------------------
    int clave_1 = 1;
    int clave_2 = 2;
    printf("Copy\n");
    if (copy_key(clave_1, clave_2) != 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en copy\n");
        return -1;
    }

    // DELETE_KEY -------------------------------------------------------------------------
    printf("Delete\n");
    if (delete_key(clave) < 0)
    { // Maneja cualquier error que se produzca al configurar el cliente
        printf("Error en delete\n");
        return -1;
    }

    return 0;
    // export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:
}