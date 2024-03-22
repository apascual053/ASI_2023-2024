#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_MESSAGE_SIZE 1024
#define KEY 0x78955476L

#define FIFO1  "/tmp/fifo_monitor_1"
#define FIFO2  "/tmp/fifo_monitor_2"

struct msg_buffer {
	long msg_type;
	char msg_text[MAX_MESSAGE_SIZE];
};

struct secreto45 {
	long secreto_tipo;
	char secreto_texto[5];
};

int menuser();
void extraerPidMonitor(int *pid, int argc, char *argv[]);

int ejercicio1(char* cad);
void ejercicio2(char* cad, int fifo1);
void ejercicio3(struct msg_buffer *msg, struct secreto45 *sec45);
void ejercicio4(struct msg_buffer *msg, struct secreto45 *sec45);
void ejercicio5(char sec6[]);
void ejercicio6(char sec[], int pid);
void ejercicio7();

void borrarCola();

int main(int argc, char** argv) {
	int pidMonitor;
	extraerPidMonitor(&pidMonitor, argc, argv);

	int fifo1;
	char cad[100], sec6[5];
	struct msg_buffer msg;
	struct secreto45 sec45;
	int opt;
	
	do{
		opt = menuser();
		switch(opt)
		{
			case 0:
				return(EXIT_SUCCESS);
				break;
			case 1:
				fifo1 = ejercicio1(cad);
				break;
			case 2:
				ejercicio2(cad, fifo1);
				break;
			case 3:
				ejercicio3(&msg, &sec45);
				break;
			case 4:
				ejercicio4(&msg, &sec45);
				break;
			case 5:
				ejercicio5(sec6);
				break;
			case 6:
				ejercicio6(sec6, pidMonitor);
				break;

			case 7:
				ejercicio7();
				break;
			case 8:
				borrarCola();
				break;
		}
	}while(1);
}

int menuser()
{
	int opt;
	int valido = 0;
	
	do{
		printf("\n");
		printf("SELECCIONA EL EJERCICIO QUE DESEAS REALIZAR.\n");
		printf("---------------------------------\n");
		printf("1.- EJERCICIO 1: Leer de un fifo con nombre.\n");
		printf("2.- EJERCICIO 2: Escribir en un fifo con nombre.\n");
		printf("3.- EJERCICIO 3: Leer de una cola de mensajes.\n");
		printf("4.- EJERCICIO 4: Escribir en una cola de mensajes.\n");
		printf("5.- EJERCICIO 5: Leer en un canal determinado.\n");
		printf("6.- EJERCICIO 6: Escribir en un canal.\n");
		printf("7.- EJERCICIO 7: Eliminar los recursos utilizados.\n");
		printf("\n0.- FIN DE LA PRUEBA\n");
		printf("\nElegir opción:");

		fflush(stdout);
		__fpurge(stdin);
		char c = getchar();
		opt = c -'0';

		if(opt >= 0 && opt <= 8)
		{
			valido = 1;
			if(opt != 0)
			{
				printf("\nRealizando el ejercicio %d...\n", opt);
			} else {
				printf("Saliendo del cliente...\n");
			}
		} else {
			printf("\nEl valor introducido no es valido. Intentelo de nuevo.\n");
		}
	} while (valido == 0);
	return(opt);
}

void extraerPidMonitor(int *pid, int argc, char *argv[])
{
	if (argc != 2)
	{
		perror("Argumento invalidos. Intentelo de nuevo.\n");
		exit(-1);
	}

	*pid = atoi(argv[1]);
	printf("PID monitor = %d\n", *pid);

	if( *pid <= 0)
	{
		perror("PID invalido. Intentelo de nuevo.\n");
		exit(-1);
	}
}

int ejercicio1(char* cad)
{
	int fifo1;
	
	fifo1 = open(FIFO1, O_RDWR);
	int n = read(fifo1, cad, 10);
	cad[n] = '\0';
	printf("¡Leido(s) %d carácter(es) del FIFO 1!\n",n);

	return(fifo1);
}

void ejercicio2(char* cad, int fifo1)
{
	int fifo2;

	mkfifo(FIFO2, 0600);
	fifo2 = open(FIFO2, O_RDWR);
	write(fifo2, cad, strlen(cad));
	printf(" Se han escrito %ld caractéres.\n", strlen(cad));

	int n = read(fifo1, cad, 10); 
	cad[n] = '\0';
	printf(" Se han leido %d caractéres.\n", n);

	write(fifo2, cad, strlen(cad)); 
	printf(" Se han escrito %ld caractéres.\n", strlen(cad));
}

void ejercicio3(struct msg_buffer* msg, struct secreto45 *sec45)
{
	int msg_id = msgget(KEY, 0666);
	if (msg_id == -1)
	{
		perror("Ejercicio 3: error en msgget.\n");
		exit(-1);
	}

	int n = msgrcv(msg_id, msg, sizeof(struct msg_buffer)-sizeof(long), 0, 0);
	if(n == -1)
	{
		perror("Ejercicio 3: error en msgrcv.\n");
		exit(-1);
	}

	printf("Mensaje recibido: Tipo %ld . Contenido: %s .\n", msg->msg_type, msg->msg_text);

	sec45->secreto_tipo = msg->msg_type;
	strcpy(sec45->secreto_texto, msg->msg_text);
}

void ejercicio4(struct msg_buffer *msg, struct secreto45 *sec45)
{
	char cadena[10]; // defino una cadena
	sprintf(cadena, "<%ld>%s",  sec45->secreto_tipo, sec45->secreto_texto);

	printf("El mensaje a enviar es: %s\n", cadena);

	// Preparo msg
	msg->msg_type = 1; //cualquier tipo
	strcpy(msg->msg_text, cadena);


   	 // Obtener el ID de la cola de mensajes o crearla si no existe
    	int msg_id;
    	msg_id = msgget(KEY, 0666 | IPC_CREAT);
    	if (msg_id == -1) {
        	perror("Ejercicio 4: error en msgget");
        	exit(-1);
    	}

    	// Poner mensaje en la cola de mensajes
    	if (msgsnd(msg_id, (void *)msg, sizeof(struct msg_buffer) - sizeof(long), 0) == -1) {
        	perror("Ejercicio 4: error en msgsnd");
        	exit(-1);
    	}
}

void ejercicio5(char sec6[])
{
	int msg_id,n;

	if ((msg_id = msgget(KEY, 0666 | IPC_CREAT)) == -1)
	{
		perror("Ejercicio5: msgget error.\n");
		exit(-1);
	}
	
	struct msg_buffer msg;
	n = msgrcv(msg_id, &msg, sizeof(struct msg_buffer) - sizeof(long), getpid(), 0);
	if (n == -1)
	{
		perror("Ejercicio5: error msgrcv.\n");
		exit(-1);
	}
	
	printf("Mensaje recibido: Tipo %ld . Contenido: %s .\n", msg.msg_type, msg.msg_text);

	sec6 = msg.msg_text;
}

void ejercicio6(char sec6[], int pid)
{
	int msg_id;

	if ((msg_id = msgget(KEY, 0666 | IPC_CREAT)) == -1)
	{
		perror("Ejercicio6: msgget error.\n");
		exit(-1);
	}

	struct msg_buffer msg;
	msg.msg_type = pid;
	strcpy(sec6, msg.msg_text);

	if(msgsnd(msg_id, &msg, sizeof(struct msg_buffer) - sizeof(long), 0))
	{
		perror("Ejercicio6: msgsnd error.\n");
		exit(-1);
	}

	printf("Se han escrito un mensaje: tipo %ld , texto: %s \n", msg.msg_type, msg.msg_text);
}

void ejercicio7(){}

void borrarCola()
{
	int msg_id = msgget(KEY, 0666 | IPC_CREAT);
	if (msg_id < 0)
	{
		perror("BorrarCola: msgget error.\n");
		exit(-1);
	}

	if (msgctl(msg_id, IPC_RMID, NULL) == -1)
	{
		perror("BorrarCola: msgctl error.\n");
		exit(-1);
	}
}
