#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_SIZE 1024
#define KEY 0x78955476L

typedef struct {
	int secret1;
	int secret2;
} Secrets;

int menuser();
void ejercicio1(int shm_id, Secrets *secrets);
void ejercicio2();
void ejercicio3();
void ejercicio4();
void ejercicio5();
void ejercicio6();
void ejercicio7();

int main(int argc, char** argv) {
        int opt, shm_id;
	Secrets secrets;

        do{
                opt = menuser();
                switch(opt)
                {
                        case 0:
                                return(EXIT_SUCCESS);
                                break;
                        case 1:
                                ejercicio1(shm_id, &secrets);
                                break;
                        case 2:
                                ejercicio2();
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

void ejercicio1(int shm_id, Secrets *secrets)
{
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
void ejercicio2(){}
void ejercicio3(){}
void ejercicio4(){}
void ejercicio5(){}
void ejercicio6(){}
void ejercicio7(){}
