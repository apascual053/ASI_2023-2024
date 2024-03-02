#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Proceso hijo 1
        close(pipefd[0]);  // Cerramos el extremo de lectura del pipe en el hijo 1
        char mensaje[BUFFER_SIZE];
        while (1) {
			printf("Introduce un mensaje: ");
            fgets(mensaje, BUFFER_SIZE, stdin);
            write(pipefd[1], mensaje, strlen(mensaje) + 1);  // Escribimos en el pipe
        }
        close(pipefd[1]);  // Cerramos el extremo de escritura del pipe en el hijo 1
    } else {  // Proceso padre
        pid = fork();
        if (pid == -1) {
            perror("Error al crear el segundo proceso hijo");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Proceso hijo 2
            close(pipefd[1]);  // Cerramos el extremo de escritura del pipe en el hijo 2
            char mensaje[BUFFER_SIZE];
            while (1) {
                read(pipefd[0], mensaje, BUFFER_SIZE);  // Leemos desde el pipe
                printf("Mensaje recibido en hijo 2: %s", mensaje);
                for(int i = 0; mensaje[i]; i++){
                    mensaje[i] = toupper(mensaje[i]);
                }
                printf("Mensaje en mayúsculas: %s", mensaje);
            }
            close(pipefd[0]);  // Cerramos el extremo de lectura del pipe en el hijo 2
        } else {  // Proceso padre
            // Esperamos a que ambos hijos terminen
            wait(NULL);
            wait(NULL);
            close(pipefd[0]);  // Cerramos el extremo de lectura del pipe en el proceso padre
            close(pipefd[1]);  // Cerramos el extremo de escritura del pipe en el proceso padre
        }
    }

    return 0;
}

