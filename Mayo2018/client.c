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
#include <netdb.h>

#define SERVER_PORT 3010
#define SERVERIP "127.0.0.1"
#define CLAVE 0x78955476L	// Clave de los recursos. Sustituir por DNI.
#define SIZE_SHM 4096		// Tamaño del segmento de memoria compartida
#define MAX_DEV	4		// Máximo # dispositivos # [1..4]
#define DEVOFFSET 200		// Offset entre dispositivos
#define MAX_SEM 5	// Número de semáforos en el array
#define HELLO	"HLO"
#define OK	"OK"
#define WRITE	'W'
#define DUMP	'D'
#define BUFLEN 256

struct shm_dev_reg{
	int estado;	/* 1 activo, cualquier otra cosa libre */
	int num_dev;	/* numero de dispositivo #[1..4] */
	char descr[15];	/* descripción del dispositivo*/
	int n_cont;	/* número de contadores para el dispositivo*/
};

struct msgq_input {
	char cmd;	/* Comando */
	int num_dev;	/* numero de dispositivo */
};

struct msgq_out{
	long type;
	char mensaje[BUFLEN];
};

int main(int argc, char *argv[])
{
    
    // CLIENTE UDP
    struct sockaddr_in listen_addr, monitor_addr;
    socklen_t size_addr = sizeof(struct sockaddr_in);
    
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct hostent *hostEnt;
    hostEnt = gethostbyname(SERVERIP);
    memcpy(&(listen_addr.sin_addr), hostEnt->h_addr, hostEnt->h_length);
    
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(SERVER_PORT);
    
    bind(sd, (struct sockaddr *) &listen_addr, size_addr);
    
    // MEMORIA COMPARTIDA
    int shm_id;
    if ((shm_id = shmget(CLAVE, SIZE_SHM, 0666 | IPC_CREAT)) == -1)
    {
        perror("Error al crear al memoria compartida.\n");
        exit(-1);
    }
    char *start = (char *)shmat(shm_id, NULL, 0);
    
    // SEMAFOROS
    int sem_id = semget(CLAVE, 4, 0666 | IPC_CREAT);
    struct sembuf up;
    struct sembuf down;

    up.sem_flg = 0;
    up.sem_op = 1;

    down.sem_flg = 0;
    down.sem_op = -1;

    struct sembuf up4[4];
    struct sembuf down4[4];

    for(int i = 1; i < 5; i++)
    {
            down4[i - 1].sem_op = -1;
            down4[i - 1].sem_flg = 0;
            down4[i - 1].sem_num = i;

            up4[i - 1].sem_op = 1;
            up4[i - 1].sem_flg = 0;
            up4[i - 1].sem_num = i;
    }

    // COLA DE MENSAJES
    int msg_id = msgget(CLAVE, 0666 | IPC_CREAT);
    struct msgq_input msg_in;
    struct msgq_out msg_out;
    
    int pid1;
    if ((pid1 = fork()) == 0)
    {
        char mensaje[BUFLEN];
        struct shm_dev_reg *dispositivo;
        int n;
        while(n = recvfrom(sd, mensaje, BUFLEN, 0, (struct sockaddr *)&monitor_addr, &size_addr))
        {
            if (mensaje[0] == 'H')
            {
                mensaje[n] = '\0';

                int num_dev, n_cont;
                char descr[BUFLEN];

                sscanf(mensaje, "HLO <%d> <%d> %s", &num_dev, &n_cont, descr);
                printf("<== %s\n", mensaje);

                up.sem_num = num_dev;
                down.sem_num = num_dev;

                semop(sem_id, &down, 1);

                dispositivo = (struct shm_dev_reg *)(start + DEVOFFSET * (num_dev - 1));

                dispositivo->estado = 1;
                dispositivo->num_dev = num_dev;
                dispositivo->n_cont = n_cont;
                strcpy(dispositivo->descr, descr);

                semop(sem_id, &up, 1);

                sprintf(mensaje, "OK <%d>", num_dev);
                printf("==> %s\n", mensaje);
                sendto(sd, mensaje, strlen(mensaje), 0, (struct sockaddr *)&monitor_addr, size_addr);
            }
            else if (mensaje[0] == 'W')
            {
                int num_dev, ind, valor;

                memcpy(&num_dev, mensaje + 2, sizeof(int));
                memcpy(&ind, mensaje + 3 + sizeof(int), sizeof(int));
                memcpy(&valor, mensaje + 4 + 2*sizeof(int), sizeof(int));
                printf("<== W <%d> <%d> <%d>\n", num_dev, ind, valor);

                up.sem_num = num_dev;
                down.sem_num = num_dev;

                semop(sem_id, &down, 1);

                int *p;
                p = (int *)(start + DEVOFFSET * (num_dev - 1) + sizeof(struct shm_dev_reg) + ind*sizeof(int));

                *p = valor;            

                semop(sem_id, &up, 1);

                sprintf(mensaje, "OK <%d>", num_dev);
                printf("==> %s\n", mensaje);
                sendto(sd, mensaje, strlen(mensaje), 0, (struct sockaddr *)&monitor_addr, size_addr);
            }
        }
    }
    else
    {
        int pid_monitor = atoi(argv[1]);
        while(msgrcv(msg_id, &msg_in, sizeof(struct msgq_input), (long)pid_monitor, 0))
		{
			printf("COMMANDO: %c DEV: %d\n", msg_in.cmd, msg_in.num_dev);
			if(msg_in.num_dev == 0)
			{
				printf("Es cero\n");
				semop(sem_id, down4, 4);
				for(int i = 1; i <= 4; i++)
				{
					msg_out.type = (long)i;
					memcpy(&msg_out.mensaje, start + DEVOFFSET * (i - 1), DEVOFFSET);
					msgsnd(msg_id, &msg_out, DEVOFFSET, 0);
				}
				semop(sem_id, up4, 4);
			}
			else
			{
				printf("Es num_dev\n");
				up.sem_num = msg_in.num_dev;
				down.sem_num = msg_in.num_dev;
					
				semop(sem_id, &down, 1);
				
				int num_dev = msg_in.num_dev;
				
				msg_out.type = (long)num_dev;
				memcpy(&msg_out.mensaje, start + DEVOFFSET * (num_dev - 1), DEVOFFSET);
				msgsnd(msg_id, &msg_out, DEVOFFSET, 0);
				
				semop(sem_id, &up, 1);
			}
		}
    }
}