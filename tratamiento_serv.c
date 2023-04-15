#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include "mensaje.h"
#include "tratamiento_serv.h"
#include "separar_mensaje.h"

#define SERVIDOR "/SERVIDOR"
#define CLIENTE "/CLIENTE"

char buffer[5];        // Buffer que almacena mensajes recibidos
struct peticion tupla; // Struct para los mensajes de petición
struct respuesta r;    // Struct para los mensajes de respuesta
pthread_mutex_t mutex;

char *buscar_clave(int clave)
{
    struct dirent *entry;
    char *path = NULL;
    FILE *archivoabrir;
    DIR *dir = opendir("data");
    if (dir == NULL)
    {
        perror("No se pudo abrir el directorio");
        return NULL;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        struct stat file_stat;
        char *filename = entry->d_name;
        printf("%s\n", filename);
        char filepath[100];
        sprintf(filepath, "data/%s", filename);
        if (stat(filepath, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }
        if (S_ISREG(file_stat.st_mode))
        {
            path = (char *)malloc(strlen(filename) + 10); // Reserva memoria dinámicamente
            sprintf(path, "data/%s", entry->d_name);

            archivoabrir = fopen(path, "r");
            if (archivoabrir == NULL) // Verificar si el archivo se pudo abrir
            {
                perror("No se pudo abrir el archivo");
                free(path);  // Liberar la memoria asignada a "path"
                path = NULL; // Resetear el puntero a NULL
                continue;
            }

            // Lee el primer elemento del archivo (la clave) y compara
            int clave_doc;
            fscanf(archivoabrir, "%d", &clave_doc);
            // Compara si son iguales, si es así corta el bucle y devuelve el nombre del archivo
            if (clave == clave_doc)
            {
                fclose(archivoabrir);
                if (closedir(dir) == -1)
                {
                    perror("closedir:");
                    free(path);  // Liberar la memoria asignada a "path"
                    path = NULL; // Resetear el puntero a NULL
                    return NULL;
                }
                return path;
            }
            fclose(archivoabrir);
        }
    }

    free(path);  // Liberar memoria dinámicamente asignada
    path = NULL; // Resetear el puntero para evitar errores

    if (closedir(dir) == -1)
    {
        perror("closedir:");
        free(path);  // Liberar la memoria asignada a "path"
        path = NULL; // Resetear el puntero a NULL
        return NULL;
    }
    return NULL; // Si no se encontró un archivo que coincida con la clave, devolver NULL
}

// ------------------------------------------------------------------------------------------------------------------
// Determina si existe un elemento con clave 'clave'. Devuelve 1 si existe, 0 si no. Devuelve -1 si en caso de error.
// ------------------------------------------------------------------------------------------------------------------
int exist(int clave)
{
    if (buscar_clave(clave) == NULL)
    {
        return 0; // No hay clave
    }
    return 1;
}

// ------------------------------------------------------------------------------------------------------------------
// Borra un elemento con clave 'clave'. Devuelve 0 en caso de éxito y -1 en caso de error (o si la clave no existe).
// ------------------------------------------------------------------------------------------------------------------
int delete_key(int clave)
{
    if (exist(clave) == 1)
    {
        char *path = buscar_clave(clave);
        if (path == NULL)
        {
            return -1; // Error en buscar clave
        }
        if (remove(path) == -1)
        {
            perror("remove");
            return -1;
        }
        free(path); // Liberar memoria asignada a "path"
    }
    else
    {
        printf("La clave no está insertada\n");
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
// Función auxiliar de init, elimina las tuplas prexistentes. Devuelve 0 en caso de éxito y -1 en caso de error.
// ------------------------------------------------------------------------------------------------------------------
int eliminar_tuplas()
{
    DIR *dir = opendir("data");
    if (dir == NULL)
    {
        perror("No se pudo abrir el directorio");
        return -1;
    }
    if (chdir("data") == -1)
    {
        perror("No se pudo cambiar al directorio 'data'");
        if (closedir(dir) == -1)
        {
            perror("closedir:");
            return -1;
        }
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char *filename = entry->d_name;
        struct stat file_stat;
        if (stat(filename, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }
        if (S_ISREG(file_stat.st_mode))
        {
            if (access(filename, W_OK) == -1)
            {
                perror("No se puede acceder al archivo");
                continue;
            }
            if (remove(filename) == -1)
            {
                perror("No se pudo eliminar el archivo");
                continue;
            }
        }
    }
    if (chdir("..") == -1)
    {
        perror("No se pudo volver al directorio anterior");

        if (closedir(dir) == -1)
        {
            perror("closedir:");
            return -1;
        }
        return -1;
    }
    closedir(dir);
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
// Inicializa el servicio. Devuelve 0 en caso de éxito y -1 en caso de error.
// ------------------------------------------------------------------------------------------------------------------
int init()
{
    if (eliminar_tuplas() == -1) // Eliminar todas las tuplas almacenadas previamente
    {
        perror("eliminar_tuplas()");
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
// Inserta un elemento. Devuelve 0 en caso de éxito y -1 en caso de error (o si la clave ya existe).
// ------------------------------------------------------------------------------------------------------------------
int set_value(int clave, char valor1[256], int valor2, double valor3)
{
    if(buscar_clave(clave)!=NULL)
    { //Si la clave existia previamente,manda error
        printf("La clave existía previamente\n");
        return -1;
    }
    DIR *dir = opendir("data");
    if (dir == NULL)
    {
        perror("Error al abrir el directorio 'data'");
        return -1;
    }
    if (closedir(dir) == -1)
    {
        perror("closedir:");
        return -1;
    }

    char filename[1024];
    snprintf(filename, 1024, "data/%d.txt", clave);

    FILE *fd = fopen(filename, "w, ccs=UTF-8");
    if (fd == NULL)
    {
        perror("Error al abrir el archivo");
        return -1;
    }

    struct peticion tupla;
    tupla.clave = clave;

    if (strlen(valor1) > 255)
    {
        fclose(fd);
        fprintf(stderr, "Error: valor de cadena de caracteres mayor al permitido\n");
        return -1;
    }

    strcpy(tupla.valor1, valor1);
    tupla.valor2 = valor2;
    tupla.valor3 = valor3;

    if (fprintf(fd, "%d,%s,%d,%lf\n", tupla.clave, tupla.valor1, tupla.valor2, tupla.valor3) < 0)
    {
        fclose(fd);
        perror("Error al escribir en el archivo");
        return -1;
    }

    if (fclose(fd) == EOF)
    {
        perror("Error al cerrar el archivo");
        return -1;
    }

    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
// Obtiene valores asociados a una key. Devuelve 0 en caso de éxito y -1 en caso de error (o si la clave no existe).
// ------------------------------------------------------------------------------------------------------------------
struct respuesta get_value(int clave)
{
    struct respuesta get;
    char *archivo = buscar_clave(clave);
    if (archivo != NULL) // Copiar los valores asociados a la clave
    {
        // Si ha encontrado el archivo,lo abre y va copiando los valores en las variables
        char *archivo_copia = (char *)malloc(strlen(archivo) + 1);
        strcpy(archivo_copia, archivo);
        FILE *archivoabrir = fopen(archivo_copia, "r");
        if (archivoabrir == NULL)
        {
            perror("fopen");
            get.respuesta = -1;
            free(archivo_copia);
            free(archivo);
            return get;
        }
        fscanf(archivoabrir, "%d,%[^,],%d,%lf", &clave, get.valor1, &get.valor2, &get.valor3);
        free(archivo_copia);
        get.respuesta = 0;
    }
    else
    {
        get.respuesta = -1;
    }
    return get;
}

// ------------------------------------------------------------------------------------------------------------------
// Modifica los valores asociados a una key. Devuelve 0 en caso de éxito y -1 en caso de error (o si la clave no
// existe).
// ------------------------------------------------------------------------------------------------------------------
int modify_value(int clave, char valor1[256], int valor2, double valor3)
{
    char *path = buscar_clave(clave);
    if (path == NULL) // Hay que comprobar que la tupla en la que se modifican valores exista, si no existe, error
    {
        fprintf(stderr, "No se encontró la clave %d\n", clave);
        return -1;
    }

    char *path_copia = strdup(path);
    if (path_copia == NULL)
    {
        fprintf(stderr, "Error al duplicar la ruta del archivo\n");
        return -1;
    }

    struct peticion tupla;
    tupla.clave = clave;

    if (strlen(valor1) > 255) // Cadena de tamaño máximo: 256 incluyendo '\0'
    {
        fprintf(stderr, "Valor de cadena de caracteres mayor al permitido\n");
        free(path_copia);
        return -1;
    }
    strcpy(tupla.valor1, valor1);
    tupla.valor2 = valor2;
    tupla.valor3 = valor3;

    // Sobreescribe la información
    FILE *archivo = fopen(path_copia, "r+"); // Abre el archivo de nuevo en modo escritura
    if (archivo == NULL)
    {
        fprintf(stderr, "No se pudo abrir el archivo %s\n", path_copia);
        free(path_copia);
        return -1;
    }

    // Mover el puntero al inicio del archivo
    fseek(archivo, 0, SEEK_SET);
    int result = fprintf(archivo, "%d,%s,%d,%f\n", tupla.clave, tupla.valor1, tupla.valor2, tupla.valor3);
    if (result < 0)
    {
        fprintf(stderr, "Error al escribir en el archivo %s\n", path_copia);
        free(path_copia);
        fclose(archivo);
        return -1;
    }

    free(path_copia);
    fclose(archivo);
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
// Copia los valores de la tupla 1 en la tupla 2. Si tupla 2 no existe, la crea. Devuelve 0 en caso de éxito y -1 en
// caso de error (o si la clave de la tupa 1 no existe).
// ------------------------------------------------------------------------------------------------------------------
int copy_key(int clave, int clave2)
{
    char v1[256];
    int v2;
    double v3;

    // 1. Comprueba si clave1 existe
    if ((exist(clave)) < 0)
    {
        perror("La clave no está insertada\n");
        return -1;
    }
    else
    {
        // Coger la info de la tupla
        if (get_value(clave).respuesta == -1)
        {
            perror("Error al tomar valores de clave");
            return -1;
        }
        strcpy(v1, get_value(clave).valor1);
        v2 = get_value(clave).valor2;
        v3 = get_value(clave).valor3;
    }

    // 2. Ver si clave2 está en el programa
    char *path2 = buscar_clave(clave2);
    if (path2 == NULL)
    {
        // Si no existe, se crea con la clave nueva un nuevo documento con la clave2 y la info del 1
        if (set_value(clave2, v1, v2, v3) == -1)
        {
            perror("Error al insertar valores de clave");
            return -1;
        }
    }
    else
    {
        // Si existe, eliminar el contenido de dentro de clave2 y añadimos los contenidos de clave
        if (modify_value(clave2, v1, v2, v3) == -1)
        {
            perror("Error al modificar valores de clave");
            return -1;
        }
    }
    return 0;
}

struct respuesta tratar_peticion(char mensaje[1024])
{
    struct peticion pet = char_to_peticion(mensaje);
    struct respuesta r;
    switch (pet.op)
    {
    case 0: // INIT
        r.respuesta = init();
        printf("Respuesta:%d\n", r.respuesta);
        break;
    case 1: // GET_VALUE
        r = get_value(pet.clave);
        break;
    case 2: // SET_VALUE
        r.respuesta = set_value(pet.clave, pet.valor1, pet.valor2, pet.valor3);
        break;
    case 3: // MODIFY_VALUE
        r.respuesta = modify_value(pet.clave, pet.valor1, pet.valor2, pet.valor3);
        break;
    case 4: // EXIST
        r.respuesta = exist(pet.clave);
        break;
    case 5: // DELETE_LEY
        r.respuesta = delete_key(pet.clave);
        break;
    case 6: // COPY_KEY
        r.respuesta = copy_key(pet.clave, pet.clave2);
        break;
    }
    return r;
}