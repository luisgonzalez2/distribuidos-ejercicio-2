#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[])
{
    // --------------------------------------------------------------------------
    // PRUEBA 7: Comprobar, usando exist, que un valor no existe
    // --------------------------------------------------------------------------

    // Borramos datos previos
    if (init() != 0)
    {
        perror("Error en init\n");
        return -1;
    }

    // Creamos datos nuevos y probamos la función exist
    int clave = 1;
    int respuesta = exist(clave);
    if (respuesta < 0)
    {
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
}
// IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente