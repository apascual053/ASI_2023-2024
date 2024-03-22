#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAX_SIZE 1024
#define KEY 0x78955476L

typedef struct {
	int secret1;
	int secret2;
} Secrets;

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int menuser();
void ejercicio1(Secrets *secrets);
void ejercicio2(Secrets *secrets);
void ejercicio3();
void ejercicio4();
void ejercicio5();
void ejercicio6();
void ejercicio7();

int main(int argc, char** argv) {
        int opt;
	Secrets secrets;

        do{
                opt = menuser();
                switch(opt)
                {
                        case 0:
                                return(EXIT_SUCCESS);
                                break;
                        case 1:
                                ejercicio1(&secrets);
                                break;
                        case 2:
                                ejercicio2(&secrets);
                                break;
                        case 3:
                                ejercicio3();
                                break;
                        case 4:
                                ejercicio4();
                                break;
                        case 5:
				ejercicio5();
				break;
                        case 6:
				ejercicio6();
                                break;
                        case 7:
				ejercicio7();
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
                printf("1.- EJERCICIO 1: Leer de la memoria compartida.\n");
                printf("2.- EJERCICIO 2: Crear memoria compartida y escribir.\n");
                printf("3.- EJERCICIO 3: Inicializar un semçaforo.\n");
                printf("4.- EJERCICIO 4: Inicializar un array de semáforos.\n");
                printf("5.- EJERCICIO 5: Leer un dato de memoria sincronizado.\n");
                printf("6.- EJERCICIO 6: Leer dato sincronizado con semáforo multiple.\n");
                printf("7.- EJERCICIO 7: Eliminar recursos utilizados.\n");
                printf("\n0.- FIN DE LA PRUEBA\n");
                printf("\nElegir opción:");

                fflush(stdout);
                __fpurge(stdin);
                char c = getchar();
                opt = c -'0';

                if(opt >= 0 && opt <= 7)
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

void ejercicio1(Secrets *secrets)
{
	int shm_id;
	void *shmaddr;

	if((shm_id = shmget(KEY, MAX_SIZE, 0400 | IPC_CREAT)) < 0)
	{
		perror("Ejercicio1: shmget error.\n");
		exit(-1);
	}
	
	if((shmaddr = shmat(shm_id, NULL, 0)) == (void *) -1)
	{
		perror("Ejercicio1: shmat error.\n");
		exit(-1);
	}

	int *p = (int *)shmaddr;
	secrets->secret1 = *p;

	p = (int *)(shmaddr + sizeof(int));
	int offset = *p;

	char *c;
	c = (char *)(shmaddr + offset);

	sscanf(c, "<%d>", &secrets->secret2);

	sleep(3);

	if (shmdt(shmaddr) == -1)
	{
		perror("Ejercicio1: smhdt error.\n");
		exit(-1);
	}

	printf("Ejercicio1: secreto 1 = %d\n", secrets->secret1);
	printf("Ejercicio1: secreto 2 = %d\n", secrets->secret2);			
}

void ejercicio2(Secrets *secrets)
{
	int shm_id, dsp;
	void *shmaddr;

	char secret1_str[8];
	char secret2_str[8];

	if((shm_id = shmget(KEY, MAX_SIZE, 0666 | IPC_CREAT)) < 0)
	{
		perror("Ejercicio2: shmget error.\n");
		exit(-1);
	}

	if((shmaddr = shmat(shm_id, NULL, 0)) == (void *) -1)
	{
		perror("Ejercicio2: shmat error.\n");
		exit(-1);
	}

	// Escribir el secreto 2 en la posicion 0
	snprintf(secret2_str, sizeof(secret2_str), "<%d>", secrets->secret2);
	strncpy((char *)shmaddr, secret2_str, sizeof(secret2_str));
	printf("Escrito secreto 2 (%s) en la posicion 0.\n", secret2_str);

	// Escribir el secreto 1 en la posicion dsp
	snprintf(secret1_str, sizeof(secret1_str), "<%d>", secrets->secret1);
	dsp = secrets->secret2 + 16;
	strncpy((char *)(shmaddr + dsp), secret1_str, sizeof(secret1_str));
	printf("Escrito secreto 1 (%s) en la posicion %d.\n", secret1_str, dsp);

	sleep(3);
	if (shmdt(shmaddr) == -1)
	{
		perror("Ejercicio2: shmdt error.\n");
		exit(-1);
	}
	printf("Ejercicio 2: desvinculado de shm.\n");
}

void ejercicio3()
{
	int sem_id;
	int key_sem = 476;

	// Crear el semáforo
	if ((sem_id = semget(KEY, 1, 0666 | IPC_CREAT)) == -1)
	{
		perror("Ejercicio3: sgmet error.\n");
		exit(-1);
	}

	// Inicializar el semáforo
	union semun arg;
	arg.val = key_sem;
	if (semctl(sem_id, 0, SETVAL, arg) == -1)
	{
		perror("Ejercico3: error semctl.\n");
	       exit(-1);
	}

	printf("Semáforo creado e inicializado con valor %d\n", key_sem);	
}

void ejercicio4()
{
	int sem_id;
	int sem_val[2] = {1,2};
	struct sembuf sem_opc[2];

	// Crear el array de semáforos
	if ((sem_id = semget(KEY, 2, 0666 | IPC_CREAT)) == -1)
	{
		perror("Ejercicio4: error semget.\n");
		exit(-1);
	}

	// Inicializar los semáforos
	for (int i = 0; i < 2; i++)
	{
		union semun arg;
		arg.val = sem_val[i];
		if (semctl(sem_id, i, SETVAL, arg) == -1)
		{
			perror("Ejercicio4: error semctl.\n");
			exit(-1);
		}

		printf("Semaforo %d inicializado con valor %d\n", i + 1, sem_val[i]);
	}
}

void ejercicio5()
{
	// Crear el semáforo
	int sem_id;
	if ((sem_id = semget(KEY, 1, 0666 | IPC_CREAT)) == -1)
	{
		perror("Ejercicio5: error semget.\n");
		exit(-1);
	}

	// Crear el segmento de memoria compartida
	int shm_id = shmget(KEY, MAX_SIZE, 0666 | IPC_CREAT);
	if (shm_id == -1)
	{
		perror("Ejercicio5: shmget error.\n");
		exit(-1);
	}

	// Asociar el segmento de momeria compartida al proceso
	void *shmaddr = shmat(shm_id, NULL, 0);
	if (shmaddr == (void *)-1)
	{
		perror("Ejercicio5: shmat error.\n");
		exit(-1);
	}

	// Sincronización con el semáforo
	struct sembuf sem_op;
	sem_op.sem_num = 0;
	sem_op.sem_op = -1; // esperar a que el semáforo esté en verde
	sem_op.sem_flg = 0;
	
	if (semop(sem_id, &sem_op, 1) == -1)
	{
		perror("Ejercicio5: semop 1 error.\n");
		exit(-1);
	}

	// Leer y modificar el dato en la memoria compartida
	int *data = (int *)shmaddr;
	int clave = *data;
	*data = -clave; // Cambiar el signo de la clave

	sleep(3);

	// Poner el semaforo en 1, desbloquear el monitor
	sem_op.sem_op = 1;
	if (semop(sem_id, &sem_op, 1) == -1)
	{
		perror("Ejercicio5: semop 2 error.\n");
		exit(-1);
	}

	printf("Clave obtenida %d\n", clave);

	// Desvincular el segmento de memoria compartida
    	if (shmdt(shmaddr) == -1) 
	{
        	perror("shmdt");
        	exit(EXIT_FAILURE);
    	}
}

void ejercicio6(){}
void ejercicio7(){}
