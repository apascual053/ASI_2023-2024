#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

void waitUser();
int enviaSenal(int pid, int senal);
void ejercicio1();
void ejercicio2(int pidMonitor);
void recibirSenal2();
void codigoPadre();
void codigoHijo1();
void codigoHijo2();

pid_t pidMonitor;

int main(int argc, char** argv)
{

	if (argc < 2)
	{
		perror("\nFalta parámetros 1.\n");
		exit(-1);
	}
	else
	{
		//averiguar pid monitor
		pidMonitor = atoi(argv[1]);
	}
	ejercicio1();
	ejercicio2(pidMonitor);
	
	//ejercicio3
	waitUser();
	enviaSenal(pidMonitor, SIGUSR1);

	sleep(1);

	int pidHijo1, pidHijo2;

	if((pidHijo1 = fork()) == -1)
	{
		perror("\nError al crear el hijo 1.\n");
		exit(-1);
	}
	else if(pidHijo1 == 0)
	{
		codigoHijo1();
	}
	else if((pidHijo2 = fork()) == -1)
	{
		perror("\nError al crear el hijo 2.\n");
		exit(-1);
	}
	else if(pidHijo2 == 0)
	{
		codigoHijo2();
	}
	else
	{
		codigoPadre();
	}
}

void waitUser()
{
	printf("Esperando al usuario...\n");
	//limpiar el buffer del teclado y el buffer de pantalla
	fflush(stdout);
	__fpurge(stdin);

	//esperar a que el usuario pulse ENTER
	getchar();
}

int enviaSenal(int pid, int senal)
{
  int codigoError = 0;

  if(kill(pid, senal) == -1)
  {
    perror("\nError en Kill.\n");
    codigoError = -1;
  }

  return(codigoError);
}

void ejercicio1()
{
	waitUser();
	enviaSenal(pidMonitor, SIGUSR1);
	sleep(3);
}

void ejercicio2(int pidMonitor)
{
	if(signal(SIGUSR2, recibirSenal2) == SIG_ERR)
	{
		perror("\nEjercicio 2: error al recibir la señal SIGUSR2.\n");
		exit(-1);
	}

	waitUser();
	enviaSenal(pidMonitor, SIGUSR1);

	sleep(3);
}

void recibirSenal2()
{
	printf("Señal SIGUSR2 recibida. Procediendo a realizar eco...\n");
	enviaSenal(pidMonitor, SIGUSR2);
	printf("Eco enviado.\n");
}

void codigoPadre()
{
	int pid, salida;
	int numProcesos = 2;

	enviaSenal(pidMonitor, SIGUSR1);
	printf("Client: ejercicio 3 terminado.\n");

	for(int p = 1; p <= numProcesos; p++)
	{
		pid = wait(&salida);
		printf("Proceso hijo %d con PID = %d terminado.\n", WEXITSTATUS(salida), pid);
	}

}

void codigoHijo1()
{
	int hijo = 1;
	printf("¡Hola! Soy el Hijo1 con PID =  %d.\n", getpid());

	pause();

	exit(hijo);
}

void codigoHijo2()
{
	int hijo = 2;
	printf("¡Hola! Soy el Hijo2 con PID =  %d.\n", getpid());

	pause();

	exit(hijo);
}

