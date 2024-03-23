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

void extraerPidMonitor(int argc, char *argv[]);
void waitUser(int *ejer);
int enviaSenal(int pid, int senal);

void codigoPadre();
void codigoH1(int *ejer);
void codigoH2(int *ejer, int pid1);

void ejercicio1(int *ejer);
void ejercicio2(int *ejer);
void ejercicio3(int *ejer);
void ejercicio3_H1();
void ejercicio2_H2();
void ejercicio4(); // h1_SIGUSR1_handler
void ejercicio4_H2(int *ejer, int pid1);
void ejercicio5(int *ejer);
void ejercicio6(int *ejer, int pid1);

void p_SIGUSR2_handler();
void h1_SIGUSR2_handler();
void h1_SIGALRM_handler();
void h2_SIGUSR1_handler();
void h2_SIGUSR2_handler();

void *funcionHilo(void *hilo);

pid_t pidMonitor;
int salir = 0, cont_threads, cont_sigusr2, estado5 = 0, cont_5 = 0;

int main(int argc, char** argv)
{
	int ejer = 0;
	
	extraerPidMonitor(argc, argv);
	
	ejercicio1(&ejer);
	ejercicio2(&ejer);
	ejercicio3(&ejer);
}

/******************** FUNCIONES BASICAS ********************/

void extraerPidMonitor(int argc, char *argv[])
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
}

void waitUser(int *ejer)
{
	(*ejer)++;
	printf("¡Pulsa enter para realizar el ejercicio: %d\n", *ejer);
	
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

/******************** EJERCICIO 1 ********************/

void ejercicio1(int *ejer)
{
	waitUser(ejer);
	enviaSenal(pidMonitor, SIGUSR1);
	//sleep(5);
}

/******************** EJERCICIO 2 ********************/

void ejercicio2(int *ejer)
{
	if(signal(SIGUSR2, p_SIGUSR2_handler) == SIG_ERR)
	{
		perror("\nEjercicio 2: error al recibir la señal SIGUSR2.\n");
		exit(-1);
	}

	waitUser(ejer);

	enviaSenal(pidMonitor, SIGUSR1);

	sleep(3);
}

void p_SIGUSR2_handler()
{
	printf("Señal SIGUSR2 recibida. Procediendo a realizar eco...\n");
	enviaSenal(pidMonitor, SIGUSR2);
	printf("Eco enviado.\n");
}


/******************** EJERCICIO 3 ********************/

void ejercicio3(int *ejer)
{
	waitUser(ejer);
	enviaSenal(pidMonitor, SIGUSR1);

	sleep(1);

	int pidH1, pidH2;

	if((pidH1 = fork()) == -1)
	{
		perror("\nError al crear el hijo 1.\n");
		exit(-1);
	}
	else if(pidH1 == 0)
	{
		codigoH1(ejer);
	}
	else if((pidH2 = fork()) == -1)
	{
		perror("\nError al crear el hijo 2.\n");
		exit(-1);
	}
	else if(pidH2 == 0)
	{
		codigoH2(ejer, pidH1);
	}
	else
	{
		codigoPadre();
	}
}

void ejercicio3_H1()
{
	int hijo = 1;
	printf("¡Hola! Soy el Hijo %d con PID =  %d (%d).\n", hijo, getpid(), getppid());
	pause();
}

void ejercicio3_H2()
{
	int hijo = 2;
	printf("¡Hola! Soy el Hijo %d con PID =  %d (%d).\n", hijo, getpid(), getppid());

	enviaSenal(pidMonitor, SIGUSR1);
	//pause();
}

/******************** EJERCICIO 4 ********************/

void ejercicio4()
{
	if(signal(SIGUSR2, h1_SIGUSR2_handler) == SIG_ERR)
	{
		perror("Hijo 1: Error al recibir la señal SIGUSR2.\n");
		exit(-1);
	}

	if(signal(SIGALRM, h1_SIGALRM_handler) == SIG_ERR)
	{
		perror("Hijo 1: Error al recibir la señal SIGALRM.\n");
		exit(-1);
	}

	printf("Preparando alarma de 20 segundos...\n");
	alarm(20);
	printf("Alarma preparada.\n");

	while(!salir)
	{
		pause();
	}

	printf("Han pasado 20 segundos.\nSe han recibido %d señales SIGUSR2.\nSe han creado %d hilos.\n", cont_sigusr2, cont_threads);
	
	pause();
}

void h1_SIGUSR2_handler()
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

void h1_SIGALRM_handler()
{
	salir = 1;
}

void ejercicio4_H2(int *ejer, int pid1)
{
	waitUser(ejer);
	
	printf("Enviando señal SIGUSR1 a %d...\n", pid1);
	enviaSenal(pid1, SIGUSR1);
	//pause();
}

/******************** EJERCICIO 5 ********************/

void ejercicio5(int *ejer)
{
	if (signal(SIGUSR1, h2_SIGUSR1_handler) == SIG_ERR)
	{
		perror("ejercicio5: SIGUSR1 error.\n");
		exit(-1);
	}

	if (signal(SIGUSR2, h2_SIGUSR2_handler) == SIG_ERR)
	{
		perror("ejercicio5: SIGUSR2 error.\n");
		exit(-1);
	}


	waitUser(ejer);

	enviaSenal(pidMonitor, SIGUSR1);
	pause();

	while(!salir)
	{
		pause();
	}
}

void h2_SIGUSR1_handler()
{
	if(estado5 == 0)
	{
		// Comienzo del ejercicio
		printf("Recibida señal SIGUSR1. Comienza el ejercicio 5...\n");
		estado5 = 1;
		salir = 0;
	}
	else if (estado5 == 1)
	{
		// Final del ejercicio
		printf("Recibina señal SIGUSR1. Finalización ejercicio 5.\n");
		printf("Se han recibido %d señales en total.\n", cont_5);
		salir = 1;
	}
}

void h2_SIGUSR2_handler()
{
	cont_5++;
	printf("Recibida señal nueva. Total: %d\n", cont_5);
	printf("Realizando eco...");
	enviaSenal(pidMonitor, SIGUSR2);
	printf("Eco realizado.\n");
}

/******************** EJERCICIO 6 ********************/

void ejercicio6(int *ejer, int pid1)
{
	waitUser(ejer);

	enviaSenal(pidMonitor, SIGUSR1);
	sleep(1);

	enviaSenal(pid1, SIGTERM);
}

/******************** PADRE ********************/

void codigoPadre()
{
	int pid, salida;
	int numProcesos = 2;

	// El proceso padre espera a que terminen lo 2 procesos hijo...
	for(int p = 1; p <= numProcesos; p++)
	{
		pid = wait(&salida);
		printf("Proceso hijo con PID = %d terminado.\n", pid);
	}
}

/******************** HIJO 1 ********************/

void codigoH1(int *ejer)
{
	if (signal(SIGUSR1, ejercicio4) == SIG_ERR)
	{
		perror("codigoH1: SIGUSR1 error.\n");
		exit(-1);
	}

	ejercicio3_H1();
	exit(1);
}

/******************** HIJO 2 ********************/

void codigoH2(int *ejer, int pid1)
{
	ejercicio3_H2();
	ejercicio4_H2(ejer, pid1);

	sleep(22); // tiempo que tarda el ejercicio 4 en realizarse... para organizar la consola

	ejercicio5(ejer);
	ejercicio6(ejer, pid1);
	exit(2);
}
