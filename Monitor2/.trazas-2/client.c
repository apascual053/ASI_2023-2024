#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define FIFO1  "/tmp/fifo_monitor_1"
#define FIFO2  "/tmp/fifo_monitor_2"

int menuser();
int ejercicio1(char* cad);
int ejercicio2(char* cad, int fifo1);

int main(int argc, char** argv) {
	int pidMonitor, fifo1, fifo2;
	char cad[100];
	int opt;
	
	do{
		opt = menuser();
		switch(opt)
		{
			case 0:
				return(EXIT_SUCCESS);
				break;
			case 1:
				fifo1 = ejercicio1(cad);
				break;
			case 2:
				fifo2 = ejercicio2(cad, fifo1);
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
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
		printf("1.- EJERCICIO 1: Leer de un fifo con nombre.\n");
		printf("2.- EJERCICIO 2: Escribir en un fifo con nombre.\n");
		printf("3.- EJERCICIO 3: Leer de una cola de mensajes.\n");
		printf("4.- EJERCICIO 4: Escribir en una cola de mensajes.\n");
		printf("5.- EJERCICIO 5: Leer en un canal determinado.\n");
		printf("6.- EJERCICIO 6: Escribir en un canal.\n");
		printf("7.- EJERCICIO 7: Eliminar los recursos utilizados.\n");
		printf("\n0.- FIN DE LA PRUEBA\n");
		printf("\nElegir opción:");

		fflush(stdout);
		__fpurge(stdin);
		char c = getchar();
		opt = c -'0';

		if(opt >= 0 && opt <= 7)
		{
			valido = 1;
			printf("\nRealizando el ejercicio %d...\n", opt);
		} else {
			printf("\nEl valor introducido no es valido. Intentelo de nuevo.\n");
		}
	} while (valido == 0);
	return(opt);
}


int ejercicio1(char* cad)
{
	int fifo1;
	
	printf("Abriendo el FIFO 1...");fifo1 = open(FIFO1, O_RDWR);printf(" Abierto correctamente\n");
	printf("Leyendo del FIFO 1...");
	int n = read(fifo1, cad, 10);
	cad[n] = '\0';
	printf(" ¡Leido(s) %d carácter(es) del FIFO 1!\n",n);

	return(fifo1);
}

int ejercicio2(char* cad, int fifo1)
{
	int fifo2;

	printf("Creando FIFO 2..."); mkfifo(FIFO2, 0600); printf(" Creado correctamente.\n");
	printf("Abriendo y escribiendo en el FIFO 2...");
	fifo2 = open(FIFO2, O_RDWR);
	write(fifo2, cad, strlen(cad));
	printf(" Se han escrito %ld caractéres.\n", strlen(cad));
	printf("Leyendo del FIFO 1..."); 
	int n = read(fifo1, cad, 10); 
	cad[n] = '\0';
	printf(" Se han leido %d caractéres.\n", n);
	printf("Escribiendo en el FIFO 2..."); 	write(fifo2, cad, strlen(cad)); printf(" Se han escrito %ld caractéres.\n", strlen(cad));
	
	return(fifo2);
}
