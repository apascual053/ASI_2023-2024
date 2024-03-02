#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

#define FIFO_CLIENTE_SERVIDOR "/tmp/fifo_cliente_servidor"
#define FIFO_SERVIDOR_CLIENTE "/tmp/fifo_servidor_cliente"

int main() {
    int fd_cliente_servidor, fd_servidor_cliente;
    char buffer[1024];

    // Crear los FIFOs
    mkfifo(FIFO_CLIENTE_SERVIDOR, 0666);
    mkfifo(FIFO_SERVIDOR_CLIENTE, 0666);

    // Abrir los FIFOs
    fd_cliente_servidor = open(FIFO_CLIENTE_SERVIDOR, O_RDONLY);
    fd_servidor_cliente = open(FIFO_SERVIDOR_CLIENTE, O_WRONLY);

    while (1) {
        // Leer del FIFO del cliente
        read(fd_cliente_servidor, buffer, sizeof(buffer));
        printf("Mensaje recibido del cliente: %s", buffer);

        // Procesar el mensaje (convertir a mayúsculas)
        for (int i = 0; buffer[i]; i++)
            buffer[i] = toupper(buffer[i]);

        // Escribir de vuelta al FIFO del cliente
        write(fd_servidor_cliente, buffer, sizeof(buffer));
    }

    // Cerrar los FIFOs
    close(fd_cliente_servidor);
    close(fd_servidor_cliente);

    return 0;
}

