#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

void codigoHijo(int memid);
void codigoPadre(int memid);

int main(int argc, char **argv)
{
  int memid;

  if( (memid=shmget(/*0x1234L*/ IPC_PRIVATE, 5*sizeof(int),0600|IPC_CREAT)) < 0){
	perror("shmget error");
	exit(-1);
  }

  if( fork() == 0 )
  {
	  codigoHijo(memid);
  } 
  else 
  {
	  codigoPadre(memid);
  }
}

void codigoHijo(int memid)
{
	int id;
  	int *nptr;
  	struct shmid_ds buff;

	if( (nptr=(int *)shmat(memid,0,0)) == (int *)-1){
		perror("shmat error");
		exit(-1);
	}

	int buffer[5] = {2,4,6,8,1};
	printf("**************  child process ***********\n");	
	
	int *p = nptr;
	for(int i = 0; i < 5; i++)
	{
		*(p + i) = buffer[i];
	}
	printf("Proceso hijo: ya he escrito los 5 enteros.\n");
	sleep(10);

	if(shmdt(nptr) < 0){
		perror("shmdt error");
		exit(-1);
	}

	if(shmctl(memid, IPC_RMID, &buff) < 0 ){
		perror("shmctl error");
		exit(-1);
	}
}

void codigoPadre(int memid)
{
	int id, salida;
  	int *nptr;
  	struct shmid_ds buff;

	if( (nptr=(int *)shmat(memid,0,0)) == (int *)-1){
		perror("shmat error");
		exit(-1);
	}

	sleep(5);
	printf("**************  father process ***********\n");
	int *p = nptr;
	for(int i = 0; i < 5; i++)
	{
		printf("%d", *(p+i));
	}
	printf("\nPadre: terminados de leer los 5 enteros.\n");	

	if(shmdt(nptr) < 0){
		perror("shmdt error");
		exit(-1);
	}

	wait(&salida);
}
