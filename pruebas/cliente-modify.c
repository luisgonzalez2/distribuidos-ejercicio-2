#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // --------------------------------------------------------------------------
    // PRUEBA 5: Modificar un valor
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

    char valor1m[256] = "Leche";
    int valor2m = 8;
    double valor3m = 4.5;
    if (modify_value(clave, valor1m, valor2m, valor3m) != 0)
    {
        perror("Error en modify\n");
        return -1;
    }

    // --------------------------------------------------------------------------
    // PRUEBA 6: Probar un valor que no existe usando get_value
    // --------------------------------------------------------------------------

    clave = 2;
    char valor1g[256];
    int valor2g;
    double valor3g;
    if (get_value(clave, valor1g, &valor2g, &valor3g) != 0)
    {
        perror("Error en get\n");
        return -1;
    }
    printf("Valor1:%s\n", valor1g);
    printf("Valor2:%d\n", valor2g);
    printf("Valor3:%lf\n", valor3g);
}
// IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente