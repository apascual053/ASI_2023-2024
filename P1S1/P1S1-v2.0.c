#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void codigoDelProcesoPadre();
void codigoDelProcesoHijo1();
void codigoDelProcesoHijo2();

int main(void)
{
	int pid1, pid2;

	if((pid1 = fork()) == -1)
	{
		perror("\nError al crear el proceso hijo 1.\n");
		exit(-1);
	}
	else if(pid1 == 0)
	{
		printf("Creando proceso hijo 1\n");
		codigoDelProcesoHijo1();
	}
	else if((pid2 = fork()) == -1)
	{
		perror("\nError al crear el proceso hijo 2.\n");
		exit(-1);
	}
	else if(pid2 == 0)
	{
		printf("Creando proceso hijo 2\n");
		codigoDelProcesoHijo2();
	}
	else
	{
		printf("Ejecutando el código del padre\n");
		codigoDelProcesoPadre();
	}
	return(0);
}


void codigoDelProcesoPadre()
{
	int p;
	int numProcesos = 2;
	int pid, salida;

	for(p = 1; p<= numProcesos; p++)
	{
		pid = wait(&salida);
		printf("\nProceso %d con PID = %d terminado.\n",WEXITSTATUS(salida), pid);
	}
}

void codigoDelProcesoHijo1()
{
	int salida = 1;
	exit(salida);
}

void codigoDelProcesoHijo2()
{
	int salida = 2;
	exit(salida);
}
