#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 1024

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_MESSAGE_SIZE];
};

int main(int argc, char *argv[]) {
    key_t key;
    int msg_id;
    struct msg_buffer msg;
    int i, num_msgs;

    // Comprueba los argumentos de línea de comandos
    if (argc < 2) {
        printf("Uso: %s <mensaje1> <mensaje2> ... <mensajeN>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Genera la clave para la cola de mensajes
    key = ftok("server_client_communication", 65);

    // Obtiene el identificador de la cola de mensajes
    msg_id = msgget(key, 0666 | IPC_CREAT);

    // Envía cada mensaje proporcionado como argumento
    for (i = 1; i < argc; i++) {
        // Configura el mensaje
        msg.msg_type = getpid();
        strcpy(msg.msg_text, argv[i]);

        // Envía el mensaje
        msgsnd(msg_id, &msg, sizeof(msg), 0);

        // Espera la respuesta del servidor
        msgrcv(msg_id, &msg, sizeof(msg), getpid() * 2, 0);

        // Muestra el mensaje traducido
        printf("Mensaje traducido: %s\n", msg.msg_text);

        // Espera 1 segundo entre mensajes (para simular trabajo desatendido)
        sleep(1);
    }

    return 0;
}

