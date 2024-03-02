#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_CLIENTE_SERVIDOR "/tmp/fifo_cliente_servidor"
#define FIFO_SERVIDOR_CLIENTE "/tmp/fifo_servidor_cliente"

int main() {
    int fd_cliente_servidor, fd_servidor_cliente;
    char buffer[1024];

    // Crear los FIFOs
    mkfifo(FIFO_CLIENTE_SERVIDOR, 0666);
    mkfifo(FIFO_SERVIDOR_CLIENTE, 0666);

    // Abrir los FIFOs
    fd_cliente_servidor = open(FIFO_CLIENTE_SERVIDOR, O_WRONLY);
    fd_servidor_cliente = open(FIFO_SERVIDOR_CLIENTE, O_RDONLY);

    while (1) {
        // Leer entrada del usuario
        printf("Introduce un mensaje: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Escribir al FIFO del servidor
        write(fd_cliente_servidor, buffer, sizeof(buffer));

        // Leer la respuesta del servidor del FIFO
        read(fd_servidor_cliente, buffer, sizeof(buffer));
        printf("Respuesta del servidor: %s", buffer);
    }

    // Cerrar los FIFOs
    close(fd_cliente_servidor);
    close(fd_servidor_cliente);

    return 0;
}

