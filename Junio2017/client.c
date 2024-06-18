#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TRUE 1
#define BUFLEN 256
#define KEY 0x78955476L
#define OFF_KA_COUNTERS 10
#define SIZE 1024
#define PORT_M 8000
#define IP_M "127.0.0.1"

#define FIFO_WR "/tmp/fifo_wr"
#define FIFO_RD "/tmp/fifo_rd"

#define HELLO_M "HELLO"
#define KA_M "KA"
#define OK_M "OK"
#define QUIT_M "QUIT"

void recogerpid(int argc, char *argv[], int *pid_m);
void userwait();
void hijo1(char *start);
void hijo2(char *start);
void hijo3(char *start);
void padre(char *start, int pid_m, int pid1, int pid2, int pid3);
void one_handler();
void two_handler();
void quit_handler();
void comprueba(int pid_m, int cont);

int sig1, sig2, sig_quit;

int main(int argc, char *argv[])
{
    int pid_m;
    recogerpid(argc, argv, &pid_m);
    userwait();
    
    // Memoria compartida
    int shm_id = shmget(KEY, SIZE, 0666 | IPC_CREAT);
    char *start = (char *)shmat(shm_id, NULL, 0);
    
    int pid1, pid2, pid3;
    if((pid1 = fork()) == -1)
    {
        perror("Error al crear el primer hijo.\n");
        exit(-1);
    }
    else if (pid1 == 0)
    {
        hijo1(start);
    }
    else if((pid2 = fork()) == -1)
    {
        perror("Error al crear el segundo hijo.\n");
        exit(-1);
    }
    else if (pid2 == 0)
    {
        hijo2(start);
    }
    else if((pid3 = fork()) == -1)
    {
        perror("Error al crear el tercer hijo.\n");
        exit(-1);
    }
    else if (pid3 == 0)
    {
        hijo3(start);
    }
    else
    {
        padre(start, pid_m, pid1, pid2, pid3);
    }
}

void recogerpid(int argc, char *argv[], int *pid_m)
{
    if (argc != 2)
    {
            perror("Argumento invalidos. Intentelo de nuevo.\n");
            exit(-1);
    }

    *pid_m = atoi(argv[1]);
    printf("PID monitor = %d\n", *pid_m);

    if( *pid_m <= 0)
    {
            perror("PID invalido. Intentelo de nuevo.\n");
            exit(-1);
    }
}

void userwait()
{
    printf("Pulsa enter para comenzar con el ejercicio...\n");
    
    fflush(stdout);
    __fpurge(stdin);
    
    getchar();
    
    return;
}

void hijo1(char *start)
{
    int fifo_rd, fifo_wr, ka_cont, secret;
    char buffer[BUFLEN];
    
    if((fifo_rd = open(FIFO_RD, O_RDWR)) == -1)
    {
        perror("Erro al abrir FIFO de lectura.\n");
        exit(-1);
    }
    if((fifo_wr = open(FIFO_WR, O_WRONLY)) == -1)
    {
        perror("Erro al abrir FIFO de escritura.\n");
        exit(-1);
    }
    
    sprintf(buffer, "hello %d", getpid());
    write(fifo_wr, buffer, strlen(buffer)+1);
    
    while(1)
    {
        read(fifo_rd, buffer, BUFLEN);
        if (strcmp(buffer, HELLO_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, KA_M) == 0)
        {
            ka_cont++;
            *(int *)(start + OFF_KA_COUNTERS) = ka_cont;
            sprintf(buffer, "ka %d", ka_cont);
            write(fifo_wr, buffer, strlen(buffer)+1);
        }
        else if (buffer[0] == 'S')
        {
            sscanf(buffer, "SECRET %d", &secret);
            sprintf(buffer, "key 2 %d", secret);
            write(fifo_wr, buffer, strlen(buffer)+1);
        }
        else if (strcmp(buffer, OK_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, QUIT_M) == 0)
        {
            sprintf(buffer, "quit");
            write(fifo_wr, buffer, strlen(buffer)+1);
            break;
        }
    }
    printf("Cerrando FIFOS...\n");
    close(fifo_wr);
    close(fifo_rd);
    
    pause();
    exit(1);
}

void hijo2(char *start)
{
    struct sockaddr_in monitor;
    struct hostent *hostEnt;
    socklen_t tam = sizeof(struct sockaddr_in);
    
    bzero((char*) &monitor, sizeof(monitor));
    
    hostEnt = gethostbyname(IP_M);
    
    memcpy(&(monitor.sin_addr), hostEnt->h_addr_list[0], hostEnt->h_length);
    monitor.sin_family = AF_INET;
    monitor.sin_port = htons(PORT_M + getppid());
    
    int idSock;
    if((idSock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
            perror("Error al crear el socket TCP.\n");
            exit(-1);
    }

    if(connect(idSock, (struct sockaddr *)&monitor, tam) == -1)
    {
            printf("Error al establecer conexión TCP.\n");
            exit(-1);
    }
    
    char buffer[BUFLEN];
    int ka_cont = 0, secret;
    
    sprintf(buffer, "hello %d", getpid());
    write(idSock, buffer, strlen(buffer)+1);
    printf("==> (TCP) %s\n", buffer);
    
    while(1)
    {
        read(idSock, buffer, BUFLEN);
        printf("<== (TCP) %s\n", buffer);
        if (strcmp(buffer, HELLO_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, KA_M) == 0)
        {
            ka_cont++;
            sprintf(buffer, "ka %d", ka_cont);
            *(int *)(start + OFF_KA_COUNTERS + sizeof(int)) = ka_cont;
            write(idSock, buffer, strlen(buffer)+1);
        }
        else if (buffer[0] == 'S')
        {
            sscanf(buffer, "SECRET %d", &secret);
            sprintf(buffer, "key 5 %d", secret);
            write(idSock, buffer, strlen(buffer)+1);
        }
        else if (strcmp(buffer, OK_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, QUIT_M) == 0)
        {
            sprintf(buffer, "quit");
            write(idSock, buffer, strlen(buffer)+1);
            printf("==> (TCP) %s\n", buffer);
            break;
        }
        printf("==> (TCP) %s\n", buffer);
    }
    printf("Cerrando socket TCP...\n");
    close(idSock);
    
    pause();
    exit(2);
}

void hijo3(char *start)
{
    struct sockaddr_in monitor;
    struct hostent *hostEnt;
    socklen_t tam = sizeof(struct sockaddr_in);
    
    bzero((char*) &monitor, sizeof(monitor));
    
    hostEnt = gethostbyname(IP_M);
    
    memcpy(&(monitor.sin_addr), hostEnt->h_addr_list[0], hostEnt->h_length);
    monitor.sin_family = AF_INET;
    monitor.sin_port = htons(PORT_M + getppid());
    
    int idSock;
    if((idSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
            perror("Error al crear el socket UDP.\n");
            exit(-1);
    }
    
    char buffer[BUFLEN];
    int ka_cont = 0, secret;
    
    sprintf(buffer, "hello %d", getpid());
    sendto(idSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&monitor, tam);
    printf("==> (UDP) %s\n", buffer);
    
    while(1)
    {
        recvfrom(idSock, buffer, BUFLEN, 0, (struct sockaddr *)&monitor, &tam);
        printf("<== (UDP) %s\n", buffer);
        if (strcmp(buffer, HELLO_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, KA_M) == 0)
        {
            ka_cont++;
            sprintf(buffer, "ka %d", ka_cont);
            *(int *)(start + OFF_KA_COUNTERS + 2*sizeof(int)) = ka_cont;
            sendto(idSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&monitor, tam);
        }
        else if (buffer[0] == 'S')
        {
            sscanf(buffer, "SECRET %d", &secret);
            sprintf(buffer, "key 8 %d", secret);
            sendto(idSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&monitor, tam);
        }
        else if (strcmp(buffer, OK_M) == 0)
        {
            continue;
        }
        else if (strcmp(buffer, QUIT_M) == 0)
        {
            sprintf(buffer, "quit");
            sendto(idSock, buffer, strlen(buffer)+1, 0, (struct sockaddr *)&monitor, tam);
            printf("==> (UDP) %s\n", buffer);
            break;
        }
        printf("==> (UDP) %s\n", buffer);
    }
    printf("Cerrando socket UDP...\n");
    close(idSock);
    
    pause();
    exit(3);
}

void padre(char *start, int pid_m, int pid1, int pid2, int pid3)
{
    signal(SIGUSR1, one_handler);
    signal(SIGUSR2, two_handler);
    signal(SIGQUIT, quit_handler);
    
    if (kill(pid_m, SIGUSR1) == -1)
    {
        perror("Padre: error al enviar primera señal SIGUSR1.\n");
        exit(-1);
    }
    else
    {
        printf("Padre: enviada señal de construcción de jerarquia.\n");
    }
    
    int cont = 0;
    int pid, salida;
    while(1)
    {
        pause();
        comprueba(pid_m, cont);
        *(int *)(start + OFF_KA_COUNTERS + 3*sizeof(int)) = cont;
        if(sig_quit == 1)
        {
            printf("Matando procesos...\n");
            kill(pid1, SIGKILL);
            kill(pid2, SIGKILL);
            kill(pid3, SIGKILL);
            
            for(int p = 0; p < 3; p++)
            {
                pid = wait(&salida);
                printf("Proceso hijo con PID = %d terminado.\n", pid);
            }
            exit(1);
        }
    }
}

void one_handler()
{
    sig1++;
    printf("Ha llegado una señal SIGUSR1.\n");
}

void two_handler()
{
    sig2++;
    printf("Ha llegado una señal SIGUSR2.\n");
}

void quit_handler()
{
    sig_quit = 1;
    printf("Ha llegado una señal SIGQUIT.\n");
}

void comprueba(int pid_m, int cont)
{
    if(sig1 == 2 && sig2 == 1)
    {
        kill(pid_m, SIGUSR2); cont++;
        printf("Eco realizado.\n");
    }
    else if ((sig1 == 1 && sig2 == 1) || (sig1 == 1 && sig2 == 0))
    {
        printf("Esperando condiciones para eco.\n");
    }
    else
    {
        sig1 = 0;
        sig2 = 0;
        printf("Contadores reseteados.\n");
    }
}




