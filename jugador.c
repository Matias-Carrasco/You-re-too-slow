// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig);

}

int main()
{
    int fd;
    int pf = 0;
    int turno;

    // FIFO file path
    char * myfifo = "/tmp/fifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr1[1024], arr2[1024];
    while (1)
    {
        signal(SIGINT, handle_sigint);
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        puts("Ingresa unas coordenadas");
        sleep(5);
        fgets(arr2, 80, stdin);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, arr2, strlen(arr2)+1);
        close(fd);

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        read(fd, arr1, sizeof(arr1));

        

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