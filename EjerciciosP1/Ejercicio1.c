#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define SI 1
#define NO 0

void codigoDelProcesoPadre();
void codigoDelProcesoHijo1();
void codigoDelProcesoHijo2();
int menu();
int enviaSenal(int pid, int senal);
void padreRecibeSenal();
void hijo1RecibeSenal();
void hijo1Sale();
void escribirMensaje(const char *mensaje);
void preguntarMensaje();
void* tareaAdicional();
void convertirAMayusculas(char *cadena);

int salir = NO;

int main(void)
{
	int pid1, pid2;

	if((pid1 = fork()) == -1)
	{
		printf("\nError al crear el proceso hijo 1.\n");
		exit(-1);
	}
	else if(pid1 == 0)
	{
		codigoDelProcesoHijo1();
	}
	else if((pid2 = fork()) == -1)
	{
		printf("\nError al crear el proceso hijo 2.\n");
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

void codigoDelProcesoPadre(void)
{
	int p;
	int numProcesos = 2;
	int pid, salida;

	if(signal(SIGUSR1, padreRecibeSenal) == SIG_ERR)
	{
		perror("\nPadre: Error en la señal SIGUSR1.\n");
		exit(-1);
	}

	for(p = 1; p <= numProcesos; p++)
	{
		pid = wait(&salida);
                int status = WEXITSTATUS(salida);
                printf("\nProceso hijo %d con PID = %d terminado.\n", status, pid);
	}
}

void codigoDelProcesoHijo1()
{
	int hijo = 1;

	if(signal(SIGUSR1, hijo1RecibeSenal) == SIG_ERR)
	{
		perror("\nHijo 1: Error en la señal SIGUSR1.\n");
		exit(-1);

	}

	if(signal(SIGUSR2, hijo1Sale) == SIG_ERR)
  {
          perror("\nHijo 1: Error en la señal SIGUSR2.\n");
          exit(-1);

  }

	while(!salir)
	{
		pause();
	}

	exit(hijo);
}

void codigoDelProcesoHijo2()
{
	int hijo = 2;

	int opcion;
	do{
		switch(opcion = menu())
		{
                    case 1:
                        if(enviaSenal(getppid(), SIGUSR1) == -1)
                        {
                            perror("\nHijo 2: Error al enviar la señal SIGUSR1 al padre.\n");
                            exit(-1);
                        }
                        break;
                    case 2:
                        if(enviaSenal(getpid()-1, SIGUSR1) == -1)
                        {
                            perror("\nHijo 2: Error al enviar la señal SIGUSR1 al hijo 1.\n");
                            exit(-1);
                        }
                        break;
                    case 3:
                        preguntarMensaje();
                        break;
		}
	}
	while(opcion != salir);

	enviaSenal(getpid()-1, SIGUSR2);

	exit(hijo);
}

int menu()
{
    int ins_corr = NO;
    int numero, op;
    char input[100];
    
    do
    {
        printf("\n¿Qué deseas hacer?\n") ;
        printf("1: Enviar señal al proceso padre.\n");
        printf("2: Enviar señal al proceso hijo 1.\n");
        printf("3: Escribir un mensaje.\n");
        printf("0: Acabar la comunicación.\n");
        printf("Tu opción: ");
        
        fgets(input, sizeof(input), stdin);
        //Convertir la cadena a número entero
        numero = atoi(input);
        //Verfificar si la conversión fue existosa
        if(numero != 0 || input[0] == '0')
        {
            ins_corr = SI;
            op = numero;
        }
        else
        {
            printf("Entrada no valida. Por favor, ingrese un número entero.\n");
        }
    }
    while(ins_corr == NO);
  
  return(op);
}

int enviaSenal(int pid, int senal)
{
  int codigoError = 0;
  
  if(kill(pid, senal) == -1)
  {
    perror("\nError en Kill.\n");
    codigoError = -1;
  }
  
  return(codigoError);
}

void padreRecibeSenal()
{
  printf("\nPadre: señal recibida.\n");
}

void hijo1RecibeSenal()
{
    printf("\nHijo 1: señal recibida.\n");
    
    // Crear un nuevo hilo para realizar una tarea adicional
    pthread_t hiloAdicional;
    if (pthread_create(&hiloAdicional, NULL, tareaAdicional, NULL) != 0) 
    {
        perror("Error al crear el hilo adicional");
        exit(-1);
    }
    
    //Esperar a que el hilo adicional termine
    if (pthread_join(hiloAdicional, NULL) != 0) 
    {
        perror("Error al esperar el hilo adicional");
        exit(-1);
    }
}

void hijo1Sale()
{
  salir = SI;
}

void escribirMensaje(const char *mensaje)
{
    FILE *archivo;
    
    archivo = fopen("mensaje.txt", "w");
    
    if (archivo == NULL)
    {
        perror("Error al abrir el fichero de mensaje.\n");
        return;
    }
    
    fprintf(archivo, "%s\n", mensaje);
    
    fclose(archivo);
    
    printf("Mensaje guardado con exito.\n");
    
    return;
}

void preguntarMensaje()
{
    char mensaje[100];
    
    printf("Ingrese un mensaje: ");
    fgets(mensaje, sizeof(mensaje), stdin);
    
    //Elimina el salto de linea final si está presente
    if (mensaje[strlen(mensaje) -1] == '\n')
    {
        mensaje[strlen(mensaje) -1] = '\0';
    }
    
    escribirMensaje(mensaje);
    
    return;
}

void* tareaAdicional() 
{
    FILE *archivo;
    char buffer[1024];

    archivo = fopen("mensaje.txt", "r");

    if (archivo == NULL) 
    {
        perror("Tarea Adicional: Error al abrir el archivo\n");
        exit(-1);
    }

    printf("Contenido del archivo en mayúsculas: ");
    while (fgets(buffer, sizeof(buffer), archivo) != NULL) 
    {
        convertirAMayusculas(buffer);
        printf("%s", buffer);
    }

    fclose(archivo);
    return(0);
}

void convertirAMayusculas(char *cadena) 
{
    while (*cadena) 
    {
        *cadena = toupper(*cadena);
        cadena++;
    }
}
