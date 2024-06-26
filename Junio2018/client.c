#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <netdb.h>

#define SERVER_PORT 3010
#define SERVERIP "127.0.0.1"
#define CLAVE 0x78955476L	// Clave de los recursos. Sustituir por DNI.
#define SIZE_SHM 4096		// Tamaño del segmento de memoria compartida
#define MAX_DEV	5		// Máximo # dispositivos # [0..4]
#define BUFLEN 256

struct shm_dev_reg{
	int estado;	/* 1 activo, cualquier otra cosa libre */
	int num_dev;	/* numero de dispositivo #[1..4] */
	char descr[16];	/* descripción del dispositivo*/
	int contador[4];	/* contadores para el dispositivo*/
};

struct msgq{
	long type;
	struct shm_dev_reg dispositivo;
};

#define MAX_SEM 5	// Número de semáforos en el array

#define HELLO	"HLO"
#define OK	"OK"
#define WRITE	'W'

#define MSQ_TYPE_BASE 100
#define DUMP_ALL -1			//Código para el dump de la totalidad de los dispositivos
#define DEV_DUMP 1500		// Posición del dispositivo del que se pide el dump


void hijo1(char *start, int sem_id, struct sembuf *up, struct sembuf *down);
void padre(char *start, int sem_id, struct sembuf *up, struct sembuf *down);
void signal_handler();

int signals;

int main(int argc, char *argv[])
{
	int pidH1, pidH2;
        
        // CREAR LA MEMORIA COMPARTIDA (EJERCICIO 2)
        int shm_id;
        if((shm_id = shmget(CLAVE, SIZE_SHM, 0666 | IPC_CREAT)) == -1)
        {
                perror("Error al crear memoria compartida: ");
                exit(-1);
        }
        char *start = (char *)shmat(shm_id, NULL, 0);
        
        // CREAR LOS SEMÁFOROS (EJERCICIO 4)
        int sem_id = semget(CLAVE, 5, 0666 | IPC_CREAT);
        struct sembuf up;
        struct sembuf down;

        up.sem_flg = 0;
        up.sem_op = 1;

        down.sem_flg = 0;
        down.sem_op = -1;
    
        // reparto de tareas 
        if ((pidH1 = fork()) == -1)
        {
            perror("Error al crear el hijo 1.\n");
            exit(-1);
        }
        else if (pidH1 == 0)
        {
            hijo1(start, sem_id, &up, &down);
        }
        else
        {
            padre(start, sem_id, &up, &down);
        }
}

void hijo1(char *start, int sem_id, struct sembuf *up, struct sembuf *down)
{
    
    // CONFIGURAR EL SOCKET DEL SERVIDOR UDP (EJERCICIO 1)
    struct sockaddr_in listen_addr, monitor_addr;
    struct hostent *hostEnt;
    socklen_t tamMonitorAddr = sizeof(monitor_addr);
    char mensaje[BUFLEN];

    int idSock = socket(AF_INET, SOCK_DGRAM, 0);

    bzero((char *) &listen_addr, sizeof(listen_addr));
    hostEnt = gethostbyname(SERVERIP);
    memcpy(&(listen_addr.sin_addr), hostEnt->h_addr, hostEnt->h_length);

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(SERVER_PORT);

    bind(idSock, (struct sockaddr *) &listen_addr, sizeof(listen_addr));
    printf("Esperando mensajes en el puerto 3001...\n");
    
    // BUCLE PRINCIPAL DEL HIJO 1
    struct shm_dev_reg *dispositivo;
    
    while(1)
    {
        recvfrom(idSock, mensaje, BUFLEN, 0, (struct sockaddr *)&monitor_addr, &tamMonitorAddr);
        if (mensaje[0] == 'H')
        {
            int num_dev;
            char descr[BUFLEN];
            
            memcpy(&num_dev, mensaje + 1, sizeof(int));
            memcpy(&descr, mensaje + 1 + sizeof(int), BUFLEN);
            
            printf("Nuevo mensaje recibido: H %d %s\n", num_dev, descr);
            
            dispositivo = (struct shm_dev_reg *)(start +sizeof(struct shm_dev_reg)*num_dev);
            
            dispositivo->estado = 1;
            dispositivo->num_dev = num_dev;
            strcpy(dispositivo->descr, descr);

            mensaje[0] = 'O';
            memcpy(mensaje + 1, &num_dev, sizeof(int));
            
            printf("El mensaje a enviar es: O %d\n", num_dev);

            sendto(idSock, mensaje, 5, 0, (struct sockaddr *)&monitor_addr, tamMonitorAddr);
        }
        else if (mensaje[0] == 'W')
        {
            int num_dev, cont, valor;

            memcpy(&num_dev, mensaje + 1, sizeof(int));
            memcpy(&cont, mensaje + 1 + sizeof(int), sizeof(int));
            memcpy(&valor, mensaje + 1 + 2*sizeof(int), sizeof(int));
            
            printf("Nuevo mensaje recibido: W %d %d %d\n", num_dev, cont, valor);
            
            up->sem_num = num_dev;
            down->sem_num = num_dev;

            semop(sem_id, down, 1);
            
            dispositivo = (struct shm_dev_reg *)(start +sizeof(struct shm_dev_reg)*num_dev);
            dispositivo->contador[cont] = valor;
            
            semop(sem_id, up, 1);

            mensaje[0] = 'O';
            memcpy(mensaje + 1, &num_dev, sizeof(int));
            
            printf("El mensaje a enviar es: O %d\n", num_dev);

            sendto(idSock, mensaje, 5, 0, (struct sockaddr *)&monitor_addr, tamMonitorAddr);
        }
        else if (mensaje [0] == 'R')
        {
            int num_dev, cont, valor;

            memcpy(&num_dev, mensaje + 1, sizeof(int));
            memcpy(&cont, mensaje + 1 + sizeof(int), sizeof(int));
            
            printf("Nuevo mensaje recibido: R %d %d\n", num_dev, cont);
            
            up->sem_num = num_dev;
            down->sem_num = num_dev;

            semop(sem_id, down, 1);
            
            dispositivo = (struct shm_dev_reg *)(start +sizeof(struct shm_dev_reg)*num_dev);
            valor = dispositivo->contador[cont];
            
            semop(sem_id, up, 1);

            mensaje[0] = 'O';
            memcpy(mensaje + 1, &num_dev, sizeof(int));
            memcpy(mensaje + 1 + sizeof(int), &valor, sizeof(int));
            
            printf("El mensaje a enviar es: O %d\n", num_dev);

            sendto(idSock, mensaje, 9, 0, (struct sockaddr *)&monitor_addr, tamMonitorAddr);
        }

    }
    
    close(idSock);
    return;
}

void padre(char *start, int sem_id, struct sembuf *up, struct sembuf *down)
{
    if(signal(SIGUSR1, signal_handler) == SIG_ERR)
    {
            perror("\nError al recibir la señal SIGUSR1.\n");
            exit(-1);
    }
    
    // CREAR COLA DE MENSAJES (EJERCICIO 5)
    int msg_id = msgget(CLAVE, 0666 | IPC_CREAT);
    struct msgq msg;
    
    while(1) // bucle infinito
    {
        pause();
        if(signal > 0) //en caso de haber recibido alguna señal SIGUSR1
        {
            int num_dev;
            memcpy(&num_dev, start + DEV_DUMP, sizeof(int));
            if(num_dev == -1)
            {
                for(int i = 0; i < 5; i++)
                {
                    up->sem_num = i;
                    down->sem_num = i;

                    semop(sem_id, down, 1);

                    msg.type = i + MSQ_TYPE_BASE;
                    msg.dispositivo = *(struct shm_dev_reg *)(start + i*sizeof(struct shm_dev_reg));

                    semop(sem_id, up, 1);

                    msgsnd(msg_id, &msg, sizeof(msg.dispositivo), 0);

                    signals = signals -1;
                }

            }
            else
            {
                up->sem_num = num_dev;
                down->sem_num = num_dev;

                semop(sem_id, down, 1);

                msg.type = num_dev + MSQ_TYPE_BASE;
                msg.dispositivo = *(struct shm_dev_reg *)(start + num_dev*sizeof(struct shm_dev_reg));

                semop(sem_id, up, 1);

                msgsnd(msg_id, &msg, sizeof(msg.dispositivo), 0);

                signals = signals -1;
            }
        }
    }
    
    int hijos = 2, salida;
    
    for(int i = 0; i < hijos; i++)
    {
        printf("Proceso con PID = %d terminado.\n", wait(&salida));
    }
}

void signal_handler()
{
    signals++;
    printf("Recibida señal SIGUSR1.\n");
}













