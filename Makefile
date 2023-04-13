# Variables
CC = gcc -g
CPPFLAGS = -I$(INSTALL_PATH)/include -Wall
LDFLAGS = -L$(INSTALL_PATH)/lib/ -L.
LDLIBS = -lclaves -lpthread -lrt -lm

# Archivos binarios a compilar
BIN_FILES = cliente servidor

# Archivos objeto
CLIENTE_OBJ_FILES = cliente.o
SERVIDOR_OBJ_FILES = servidor.o tratamiento_serv.o lines.o separar_mensaje.o
LIB_OBJ_FILES = claves.o

# Reglas
all: $(BIN_FILES)
# Crear biblioteca compartida de objetos
libclaves.so: claves.o lines.o separar_mensaje.o
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Crear ejecutables
cliente: $(CLIENTE_OBJ_FILES) libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

servidor: $(SERVIDOR_OBJ_FILES) 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.c claves.h lines.h separar_mensaje.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o *.so

#export INSTALL_PATH=/home/elisa/Documentos/distribuidos-ejercicio-2-1
#export LD_LIBRARY_PATH=/home/elisa/Documentos/distribuidos-ejercicio-2-1

