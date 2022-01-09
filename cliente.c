
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

//Memoria compartida
#include <sys/stat.h>
#include <sys/mman.h>

 int fd;
 int conexion;

char * myfifo = "/tmp/fifo";

void handle_sigint(int sig)
{
    printf("Te declareste ganador\n");

    fd = open(myfifo, O_WRONLY); 
    kill(conexion,SIGINT);
    
    puts("El juego terminado!\n");
    exit(0);

}

int main()
{
   

    int pf = 0;
    int turno;

    mkfifo(myfifo, 0666);

    fd = open(myfifo, O_WRONLY);    
    char msj[20];
    int pid = getpid();
    sprintf(msj,"%d",pid); 
    write(fd,msj,20);
    close(fd);

    char res[20];
    fd = open(myfifo, O_RDONLY);
    read(fd,res,20);
    close(fd);
    conexion = atoi(res);
            



    puts("Coordenadas aceptadas entre [0,3]\n");

   
    while (1)
    {
        char arr1[1024], arr2[1024];
        signal(SIGINT, handle_sigint);
    
        fd = open(myfifo, O_WRONLY);

        puts("Ingresa unas coordenadas");
        
        fgets(arr2, 80, stdin);


        write(fd, arr2, strlen(arr2)+1);
        fsync(fd);
        close(fd);
        
     

        fd = open(myfifo, O_RDONLY);

        read(fd, arr1, sizeof(arr1));
        fsync(fd);
        printf("%s\n", arr1);
        
        close(fd);

 


    }
    return 0;
}