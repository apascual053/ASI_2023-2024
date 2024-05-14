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
void ejercicio3(int *idSockCli, char *mensaje);
void ejercicio4(int *idSockServ, int *idSockCli);
void ejercicio5(int *idSockCli);
void ejercicio6(int *idSockCli);
void ejercicio7(int *idSockServ, int *idSockCli);

int main(int argc, char** argv) {
        int opt, idSockCli, idSockServ;
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
                                ejercicio3(&idSockCli, mensaje);
                                break;
                        case 4:
                                ejercicio4(&idSockServ, &idSockCli);
                                break;
                        case 5:
				ejercicio5(&idSockCli);
				break;
                        case 6:
				ejercicio6(&idSockCli);
                                break;
                        case 7:
				ejercicio7(&idSockServ, &idSockCli);
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
                                printf("Agur\n");
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

void ejercicio4(int *idSockServ, int *idSockCli)
{
	struct sockaddr_in dirServ, dirCli;
	struct hostent *hostEnt;
	socklen_t tamDirCli = sizeof(dirCli);

	*idSockServ = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *) &dirServ, sizeof(dirServ));

	hostEnt = gethostbyname("localhost");
	memcpy(&(dirServ.sin_addr), hostEnt->h_addr, hostEnt->h_length);

	dirServ.sin_family = AF_INET;
	dirServ.sin_port = htons(3001);

	bind(*idSockServ, (struct sockaddr *) &dirServ, sizeof(dirServ));

	listen(*idSockServ, 1);
	printf("Esperando conexión en el puerto 3001...\n");

	*idSockCli = accept(*idSockServ, (struct sockaddr *) &dirCli, &tamDirCli);
	printf("Conexión aceptada.\n");

}

void ejercicio5(int *idSockCli)
{
	char mensaje [MAX_TAM];

	int n = read(*idSockCli, (char *)mensaje, sizeof(mensaje));
	mensaje[n] = '\0';
	printf("El secreto 5 es: %s\n", mensaje);

	write(*idSockCli, (char *)mensaje, sizeof(mensaje));
	printf("Secreto 5 enviado de vuelta.\n");
}

void ejercicio6(int *idSockCli)
{
	struct sockaddr_in dirServ;
	struct hostent *hostEnt;
	socklen_t tamDirServ = sizeof(dirServ);

	char mensaje[] = "HOLA!";

	*idSockCli = socket(AF_INET, SOCK_DGRAM, 0);

	bzero((char *) &dirServ, sizeof(dirServ));
	
	hostEnt = gethostbyname("localhost");
	memcpy(&(dirServ.sin_addr), hostEnt->h_addr, hostEnt->h_length);

	dirServ.sin_family = AF_INET;
	dirServ.sin_port = htons(3000);

	sendto(*idSockCli, mensaje, strlen(mensaje), 0, (struct sockaddr *)&dirServ, tamDirServ);
	printf("Saludo enviado al servidor.\n");
}

void ejercicio7(int *idSockServ, int *idSockCli)
{
	struct sockaddr_in dirServ, dirCli;
	struct hostent *hostEnt;
	socklen_t tamDirCli = sizeof(dirCli);

	char mensaje[MAX_TAM];

	*idSockCli = socket(AF_INET, SOCK_DGRAM, 0);

	bzero((char *) &dirServ, sizeof(dirServ));

        hostEnt = gethostbyname("localhost");
        memcpy(&(dirServ.sin_addr), hostEnt->h_addr, hostEnt->h_length);
	
        dirServ.sin_family = AF_INET;
        dirServ.sin_port = htons(3001);

	bind(*idSockCli, (struct sockaddr *) &dirServ, sizeof(dirServ));
	printf("Esperando mensajes en el puerto 3001...\n");

	ssize_t n = recvfrom(*idSockCli, mensaje, MAX_TAM, 0, (struct sockaddr *)&dirCli, &tamDirCli);		
	mensaje[n] = '\0';
	printf("Mensaje recibido: %s\n", mensaje);
		
	sendto(*idSockCli, mensaje, strlen(mensaje), 0, (struct sockaddr *)&dirCli, tamDirCli);
        printf("Eco enviado.\n");

	close(*idSockCli);
}
