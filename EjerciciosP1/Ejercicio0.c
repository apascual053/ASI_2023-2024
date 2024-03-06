#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int entero_global = 0;

void codigoDelProcesoPadre(int entero_main);
void codigoDelProcesoHijo1(int entero_main);
void codigoDelProcesoHijo2(int entero_main);

int main(void)
{
	int pid1, pid2;
	int entero_main;

	if((pid1 = fork()) == -1)
	{
		printf("\nError al crear el proceso hijo 1.\n");
		exit(-1);
	}
	else if(pid1 == 0)
	{
		codigoDelProcesoHijo1(entero_main);
	}
	else if((pid2 == fork()) == -1)
	{
		printf("\nError al crear el proceso hijo 2.\n");
		exit(-1);
	}
	else if(pid2 == 0)
	{
		codigoDelProcesoHijo2(entero_main);
	}
	else
	{
		codigoDelProcesoPadre(entero_main);
	}
	return(0);
}


void codigoDelProcesoPadre(int entero_main)
{
	int p;
	int numProcesos = 2;
	int pid, salida;
	
	//esperar a que acaben los dos procesos hijos
	for(p = 1; p <= numProcesos; p++)
	{
		pid = wait(&salida);
		printf("\nProceso %d con PID = %d terminado.\n", WEXITSTATUS(salida), pid);
	}
}

void codigoDelProcesoHijo1(int entero_main)
{
	int entero_local = 0;
	for(int i = 0; i < 100000; i++)
	{
		entero_local++; entero_main++; entero_global++;
		printf("Entero local: %d\n", entero_local);
		printf("Entero main: %d\n", entero_main);
		printf("Entero global: %d\n\n", entero_global);
	}
	exit(1);
}

void codigoDelProcesoHijo2(int entero_main)
{
	int entero_local = 0;
	for(int i = 0; i < 100000; i++)
	{
		entero_local++; entero_main++; entero_global++;
		printf("			Entero local: %d\n", entero_local);
		printf("			Entero main: %d\n", entero_main);
		printf("			Entero global: %d\n\n", entero_global);
	}
	exit(2);
}
