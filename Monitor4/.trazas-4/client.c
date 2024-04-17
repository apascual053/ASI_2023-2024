#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_TAM 100

int menuser();
void ejercicio1(int *idSockCli);
void ejercicio2(int *idSockCli, char *mensaje);
void ejercicio3();
void ejercicio4();
void ejercicio5();
void ejercicio6();
void ejercicio7();

int main(int argc, char** argv) {
        int opt, idSockCli;
	char mensaje[MAX_TAM];

        do{
                opt = menuser();
                switch(opt)
                {
                        case 0:
                                return(EXIT_SUCCESS);
                                break;
                        case 1:
                                ejercicio1(&idSockCli);
                                break;
                        case 2:
                                ejercicio2(&idSockCli, mensaje);
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
                printf("1.- EJERCICIO 1: Establecer conexión TCP.\n");
                printf("2.- EJERCICIO 2: Recibir datos por TCP.\n");
                printf("3.- EJERCICIO 3: Enviar datos por TCP.\n");
                printf("4.- EJERCICIO 4: Recibir conexión en servidor TCP.\n");
                printf("5.- EJERCICIO 5: Recibir/Enviar datos en servidor TCP.\n");
                printf("6.- EJERCICIO 6: Enviar datos por UDP.\n");
                printf("7.- EJERCICIO 7: Recibir datos en servidor UDP.\n");
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

void ejercicio1(int *idSockCli)
{
	struct sockaddr_in dirServ;
	struct hostent *hostEnt;
	
	*idSockCli = socket(AF_INET, SOCK_STREAM, 0);

	bzero ((char *) &dirServ, sizeof(dirServ));

	hostEnt = gethostbyname("localhost");
	memcpy(&(dirServ.sin_addr), hostEnt->h_addr, hostEnt->h_length);

	dirServ.sin_family = AF_INET;
	dirServ.sin_port = htons(3000);

	connect (*idSockCli, (struct sockaddr*)&dirServ, sizeof(dirServ));
   
    	printf("CONECTADO\n");
}

void ejercicio2(int *idSockCli, char *mensaje)
{
	int n = read(*idSockCli, (char *)mensaje, sizeof(mensaje));
	mensaje[n] = '\0';
	printf("El secreto 2 es: %s\n", mensaje);
}

void ejercicio3(int *idSockCli, char *mensaje)
{
	write(*idSockCli, (char *)mensaje, sizeof(mensaje));
	printf("Secreto 2 enviado...\n");
	int n = read(*idSockCli, (char *)mensaje, sizeof(mensaje));
	mensaje[n] = '\0';
	printf("El secreto tres recibido: %s", mensaje);
	write(*idSockCli, (char *)mensaje, sizeof(mensaje));
}
void ejercicio4(){}
void ejercicio5(){}
void ejercicio6(){}
void ejercicio7(){}
