#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // --------------------------------------------------------------------------
    // PRUEBA 8: Copiar una clave a otra que PREVIAMENTE EXISTE
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

    if (copy_key(clave, clave2) != 0)
    {
        perror("Error en copy\n");
        return -1;
    }

    // --------------------------------------------------------------------------
    // PRUEBA 9: Intentar copiar una clave a otra que PREVIAMENTE NO EXISTE
    // --------------------------------------------------------------------------

    int clave3 = 3;
    if (copy_key(clave, clave3) != 0)
    {
        perror("Error en copy\n");
        return -1;
    }

    // --------------------------------------------------------------------------
    // PRUEBA 10: Intentar copiar una clave que NO EXISTE a otra que si
    // --------------------------------------------------------------------------

    int clave4 = 4;
    if (copy_key(clave4, clave3) != 0) // Devolverá error, clave 4 no tiene valores.
    {
        perror("Error en copy\n");
        return -1;
    }
}
// IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente