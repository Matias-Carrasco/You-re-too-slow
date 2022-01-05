
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


void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig);

}

int main()
{

    //que pasa si creo el file descriptor en memoria compartida
    int fd;

    int pf = 0;
    int turno;
    int bandera=0;

    // FIFO file path
    char * myfifo = "/tmp/fifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char pid[20];
    sprintf(pid, "%d", getpid());

    fd = open(myfifo, O_WRONLY);
    write(fd, pid, 20);
    close(fd);

    char arr1[1024], arr2[1024];
    while (1)
    {
        //signal(SIGINT, handle_sigint);
        // Open FIFO for write only
        
        if(bandera==0){
            fd = open(myfifo, O_WRONLY);
            puts("Escriba jugar para jugar");
            fgets(arr2,80,stdin);
            
            sprintf(arr2, "%d", getpid()); 
            write(fd, arr2, strlen(arr2)+1);
            close(fd);
            bandera=1;
        }

        fd = open(myfifo, O_WRONLY);
        // Take an input arr2ing from user.
        // 80 is maximum length
        puts("Ingresa unas coordenadas");
        sleep(1);
        fgets(arr2, 80, stdin);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, arr2, strlen(arr2)+1);
        close(fd);

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        read(fd, arr1, sizeof(arr1));
        bandera=0;


        // Print the read message
        printf("%s\n", arr1);
        close(fd);
        for(int i=0;i<strlen(arr1);i++){
            if (arr1[i]==':')
            {
                
            int a= arr1[i+2]- '0';
            int b= arr1[i+3]- '0';
            if(arr1[i+3] != '\0'){
                pf = (a*10)+b;
                printf("Valor Cambiado %d\n",pf);
                

            }else{
                printf("Falta el valor para b\n");
            }

        
            break; 
            }  
        }



       
        if(pf >= 20){
                printf("Felicidades haz ganado! puntaje total %d\n",pf);
                exit(1);
            }
    }
    return 0;
}