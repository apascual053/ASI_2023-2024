#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	int p;
	int id[NUM_PROCESOS] = {1,2,3,4,5}
	int pid;

	for(p = 0; p < NUM_PROCESOS; p++)
	{
		if((pid = fork()) == -1)
		{
			perror("\nError al crear el proceso hijo.\n");
			exit(-1);
		}
		else if(pid == 0)
		{
			codigoDelProcesoHijo(id[p]);
		}
	}
	codigoDelProcesoPadre();
	return(0);
}
