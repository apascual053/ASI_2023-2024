#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 1024

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_MESSAGE_SIZE];
};

void to_uppercase(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}

int main() {
    key_t key;
    int msg_id;
    struct msg_buffer msg;

    // Genera la clave para la cola de mensajes
    key = ftok("server_client_communication", 65);

    // Crea la cola de mensajes
    msg_id = msgget(key, 0666 | IPC_CREAT);

    while (1) {
        // Recibe el mensaje de cualquier tipo
        msgrcv(msg_id, &msg, sizeof(msg), 0, 0);

        // Procesa el mensaje (convierte a mayúsculas)
        to_uppercase(msg.msg_text);

        // Envía la respuesta de vuelta al cliente
        msg.msg_type = msg.msg_type * 2;
        msgsnd(msg_id, &msg, sizeof(msg), 0);
    }

    // Elimina la cola de mensajes
    msgctl(msg_id, IPC_RMID, NULL);

    return 0;
}

