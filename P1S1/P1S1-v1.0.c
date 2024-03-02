#include <sys/types.h>
#include <unistd.h>

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
		codigoDelProcesoHijo1();
	}
	else if((pid2 == fork()) == -1)
	{
		perror("\nError al crear el proceso hijo 2.\n");
		exit(-1);
	}
	else if(pid2 == 0)
	{
		codigoDelProcesoHijo2();
	}
	else
	{
		codigoDelProcesoPadre();
	}
	return(0);
}
