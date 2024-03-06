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
void codigoPadre(int pid1, int pid2);
void codigoHijo1();
void codigoHijo2();
void crearHilo();
void *funcionHilo();
void pararBucle();
void ejercicio5();
void atencion5();

pid_t pidMonitor;
int cont_threads;
int cont_sigusr2;
int salir = 0;
int estado5 = 0;
int cont_5;

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
		codigoPadre(pidHijo1, pidHijo2);
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
	sleep(5);
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

void codigoPadre(int pid1, int pid2)
{
	int pid, salida;
	int numProcesos = 2;

	//Ejercicio 4 (client)
	waitUser();
	sleep(22);

	//Ejercicio 5 (client)
	waitUser();
	enviaSenal(pidMonitor, SIGUSR1);

	//Ejercicio 6 (client)
	waitUser();
	enviaSenal(pidMonitor,SIGUSR1);
	sleep(1);
	enviaSenal(pid1,SIGUSR2);
	enviaSenal(pid2,SIGUSR2);
	for(int p = 1; p <= numProcesos; p++)
	{
		pid = wait(&salida);
		printf("Proceso hijo %d con PID = %d terminado.\n", WEXITSTATUS(salida), pid);
	}

}

void codigoHijo1()
{
	if(signal(SIGUSR2, crearHilo) == SIG_ERR)
	{
		perror("Hijo 1: Error al recibir la señal SIGUSR2.\n");
		exit(-1);
	}

	if(signal(SIGALRM, pararBucle) == SIG_ERR)
	{
		perror("Hijo 1: Error al recibir la señal SIGALRM.\n");
		exit(-1);
	}

	int hijo = 1;
	printf("¡Hola! Soy el Hijo %d con PID =  %d.\n",hijo,  getpid());

	//Ejercicio 3 (Hijo 1)
	pause();

	//Ejercicio 4 (Hijo 1)
	printf("Preparando alarma de 20 segundos...\n");
	alarm(20);
	printf("Alarma preparada.\n");
	while(!salir)
	{
		pause;
	}

	printf("Han pasado 20 segundos.\nSe han recibido %d señales SIGUSR2.\nSe han creado %d hilos.\n", cont_sigusr2, cont_threads);

	pause();

	exit(hijo);
}

void codigoHijo2()
{
	if(signal(SIGUSR1, ejercicio5) == SIG_ERR)
	{
		perror("Hijo 2: Error al recibir la señal SIGUSR1.\n");
		exit(-1);
	}

	if(signal(SIGUSR2, atencion5) == SIG_ERR)
	{
		perror("Hijo 2: Error al recibir señal SIGUSR2.\n");
		exit(-1);
	}

	int hijo = 2;
	printf("¡Hola! Soy el Hijo %d con PID =  %d.\n", hijo, getpid());

	enviaSenal(pidMonitor, SIGUSR1);

	pause();
	while(!salir)
	{
		pause();
	}

	pause();

	exit(hijo);
}

void crearHilo()
{
	pthread_t hilo;
	int error;

	cont_sigusr2++;
	printf("Recibida señal SIGUSR2.\n");
	error = pthread_create(&hilo, NULL, funcionHilo, (void *)&hilo);
	if(error)
	{
		perror("Error al crear el hilo.\n");
		exit(-1);
	}
	cont_threads++;

	/*error = pthread_kill(hilo, NULL);
	if(error)
	{
		perror("Error al esperar terminación del hilo");
		exit(-1);
	}*/
}

void *funcionHilo(void *hilo)
{
	pthread_t idHilo = *((pthread_t *)hilo);
	int num_hilo = cont_threads;
	printf("Hola soy el hilo %d con id = %ld.\n", cont_threads, idHilo);
	
	for (int i = 1; i < 5; i++)
	{
		sleep(5);
		printf("Soy el hijo %d y llevo activo %d segundos.\n",num_hilo, i*5);
	}

	pthread_exit(NULL);
}

void pararBucle()
{
	salir = 1;
}

void ejercicio5()
{
	if(estado5 == 0)
	{
		printf("Recibina señal SIGUSR1. Comienza el ejercicio 5...\n");
		estado5 = 1;
		salir = 0;
	}
	else if (estado5 == 1)
	{
		printf("Recibina señal SIGUSR1. Finalización ejercicio 5.\n");
		printf("Se han recibido %d señales en total.\n", cont_5);
		salir = 1;
		estado5++;
	}
	else if (estado5 == 2)
	{
		printf("\n");
	}
}

void atencion5()
{
	cont_5++;
	printf("Recibida señal nueva. Total: %d\n", cont_5);
	printf("Realizando eco...");
	enviaSenal(pidMonitor, SIGUSR2);
	printf("Eco realizado.\n");
}
