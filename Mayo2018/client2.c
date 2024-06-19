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

#define SERVER_PORT 3010
#define SERVERIP "127.0.0.1"

#define CLAVE 0x78955476L	// Clave de los recursos. Sustituir por DNI.

#define SIZE_SHM 4096		// Tamaño del segmento de memoria compartida

#define MAX_DEV	4		// Máximo # dispositivos # [1..4]

#define DEVOFFSET 200		// Offset entre dispositivos

#define BUFLEN 256

/* El dispositivo 1 comienza en la posición 0 de la memoria
	El dispositivo 2 en la posición 0+OFFSET
	El dispositivo 3 en OFFSET*2
	El dispositivo 4 en OFFSET*3
*/

/* Formato de un registro de dispositivo */

struct shm_dev_reg{
	int estado;	/* 1 activo, cualquier otra cosa libre */
	int num_dev;	/* numero de dispositivo #[1..4] */
	char descr[15];	/* descripción del dispositivo*/
	int n_cont;	/* número de contadores para el dispositivo*/
};

/* Detrás del registro del dispositivo (+sizeof(struct shm_dev_reg)) 
   se almacenan uno tras otro sus valores en formato (int)
*/


/* El array de semáforos a crear tiene tamaño 5
	El semáforo 0 no se utilizará
	El semáforo 1 controla el acceso al registro del dispositivo 1 
	El semáforo # controla el acceso al registro del dispositivo # [1..4]
*/
#define MAX_SEM 5	// Número de semáforos en el array

/* Lista de mensajes UDP */
#define HELLO	"HLO"
#define OK	"OK"
#define WRITE	'W'

/* Comandos cola de mensajes */
#define DUMP	'D'

/* Estructura para la lectura de comando + dispositivo: Total 8 bytes */

struct msgq_input {
	long type;
	char cmd;	/* Comando */
	int num_dev;	/* numero de dispositivo */
};

struct msgq_out{
	long type;
	char mensaje[BUFLEN];
};



int main(int argc, char *argv[])
{
	int pid, status;
	char buffer[BUFLEN];
	int pid_monitor = atoi(argv[1]);
	
	//Memoria compartida
	int shm_id = shmget(CLAVE, SIZE_SHM, 0666 | IPC_CREAT);
	char *start = (char *)shmat(shm_id, NULL, 0);
	
	//Semaforos
	/*int sem_id = semget(CLAVE, 4, 0666 | IPC_CREAT);
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
	
	//Cola de mensajes
	int msg_id = msgget(CLAVE, 0666 | IPC_CREAT);
	struct msgq_input msg_in;
	struct msgq_out msg_out;*/
	
	//Serv UDP
	int sd;
	struct sockaddr_in monitor_addr;
	struct sockaddr_in listen_addr;
	socklen_t size_addr = sizeof(struct sockaddr_in);
	
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = inet_addr(SERVERIP);
	listen_addr.sin_port = htons(SERVER_PORT);
	
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Error socket: ");
		exit(EXIT_FAILURE);
	}
	
	if(bind(sd, (struct sockaddr *)&listen_addr, size_addr) == -1)
	{
		perror("Error conexion: ");
		exit(EXIT_FAILURE);
	}
	
	int n;
	struct shm_dev_reg *dispositivo;
	
	if((pid = fork()) == 0)
	{
		while(n = recvfrom(sd, buffer, BUFLEN, 0, (struct sockaddr *)&monitor_addr, &size_addr))
		{
			if(buffer[0] == 'H')
			{
				buffer[n] = '\0';
				
				int num_dev, n_cont;
				char descr[BUFLEN];
				sscanf(buffer, "HLO <%d> <%d> %s", &num_dev, &n_cont, descr);
				
				//up.sem_num = num_dev;
				//down.sem_num = num_dev;
				
				//semop(sem_id, &down, 1);
				
				dispositivo = (struct shm_dev_reg *)(start + DEVOFFSET * (num_dev - 1));
				
				dispositivo->estado = 1;
				dispositivo->num_dev = num_dev;
				dispositivo->n_cont = n_cont;
				strcpy(dispositivo->descr, descr);
				
				//semop(sem_id, &up, 1);
				
				sprintf(buffer, "OK <%d>", num_dev);
			}
			/*else if(buffer[0] == 'W')
			{
				int num_dev, indice, valor;
				memcpy(&num_dev, buffer + 1, sizeof(int));
				memcpy(&indice, buffer + 1 + sizeof(int), sizeof(int));
				memcpy(&valor, buffer + 1 + 2*sizeof(int), sizeof(int));
				
				up.sem_num = num_dev;
				down.sem_num = num_dev;
				
				semop(sem_id, &down, 1);
				
				*(int *)(start + DEVOFFSET * (num_dev - 1) + indice*sizeof(int) + sizeof(struct shm_dev_reg)) = valor;
				
				semop(sem_id, &up, 1);
				
				sprintf(buffer, "OK <%d>", num_dev);
			}*/
			sendto(sd, buffer, strlen(buffer), 0, (struct sockaddr *)&monitor_addr, size_addr);
		}
		
		exit(EXIT_SUCCESS);
	}
	else
	{
		/*while(msgrcv(msg_id, &msg_in, sizeof(struct msgq_input), (long)pid_monitor, 0))
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
		}*/
	}
	waitpid(pid, &status, 0);
	
	return 0;
}


















