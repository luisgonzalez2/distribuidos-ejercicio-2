#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // --------------------------------------------------------------------------
    // PRUEBA 11: Crear una clave y borrarla
    // --------------------------------------------------------------------------

    // Borramos datos previos
    if (init() != 0)
    {
        perror("Error en init\n");
        return -1;
    }

    // Creamos datos nuevos
    char valor1[256] = "Hola";
    int valor2 = 4, clave = 1;
    double valor3 = 1.2;
    if (set_value(clave, valor1, valor2, valor3) != 0)
    {
        perror("Error en set\n");
        return -1;
    }

    char valor1_2[256] = "Mandarina";
    int valor2_2 = 7, clave2 = 2;
    double valor3_2 = 4.9;
    if (set_value(clave2, valor1_2, valor2_2, valor3_2) != 0)
    {
        perror("Error en set\n");
        return -1;
    }

    if (delete_key(clave) < 0)
    {
        perror("Error en delete\n");
        return -1;
    }

    // --------------------------------------------------------------------------
    // PRUEBA 12: Intentar borrar una clave que NO EXISTE
    // --------------------------------------------------------------------------

    int clave3 = 3;
    if (delete_key(clave3) < 0)
    {
        perror("Error en delete\n");
        return -1;
    }
}
// IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente