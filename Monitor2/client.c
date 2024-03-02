#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_PATH "/tmp/fifo_monitor_1"

// Declaración de funciones
void ejercicio_1_y_2();

// Función principal
int main() {
    // Llamar a la función que realiza los ejercicios 1 y 2
    ejercicio_1_y_2();

    return 0;
}

// Definición de funciones

// Función para realizar los ejercicios 1 y 2
void ejercicio_1_y_2() {
    int fd;
    char secret_message[10];

    // Crear el named pipe (fifo)
    mkfifo(FIFO_PATH, 0666);

    printf("Esperando a que el cliente abra el dispositivo...\n");

    // Abrir el named pipe para lectura
    fd = open(FIFO_PATH, O_RDONLY);

    // Verificar si se pudo abrir correctamente
    if (fd == -1) {
        perror("Error al abrir el dispositivo");
        exit(EXIT_FAILURE);
    }

    // Leer el mensaje secreto del named pipe
    read(fd, secret_message, sizeof(secret_message));

    // Cerrar el named pipe después de la lectura
    close(fd);

    // Imprimir el mensaje secreto leído
    printf("Mensaje secreto: %s\n", secret_message);
}

