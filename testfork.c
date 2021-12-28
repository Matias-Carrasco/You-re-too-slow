
// C program to implement one side of FIFO
// This side reads first, then reads

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

 

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
 #include <sys/wait.h> //wait pid




int main()
{
    int fd1; 
    pid_t hijo1; 
    pid_t hijo2; 
    pid_t hijo3; 
    pid_t hijo4; 
 
    char *myfifo = "/tmp/myfifotest1";

    mkfifo(myfifo, 0666);
    int max = 1024;
    char str1[max], str2[max];

    hijo1=fork();


    if(hijo1 == 0)
    {
        while(1){
             //printf("Soy el hijo 1\n");
            fd1 = open(myfifo, O_WRONLY);

            strcat(str1, "Escribe tu nombre");
            write(fd1, str1, strlen(str1) + 1);
            close(fd1);

            fd1 = open(myfifo, O_RDONLY);
                
            read(fd1, str2, sizeof(str2));
            close(fd1);
            if(str2!=NULL){
                printf("hijo 1: A ingresado el usuario: %s\n", str2);
                break;
            }
          
        }
        printf("Se cierra el hijo 1\n");
    
        exit(EXIT_SUCCESS);
    }else{

        hijo2 = fork();
        if(hijo2==0){
           
            exit(EXIT_SUCCESS);
        }else{
             hijo3 = fork();
            if(hijo3==0){
                
                exit(EXIT_SUCCESS);
            }else{
                hijo4 = fork();
                if(hijo4==0){
                   
                    exit(EXIT_SUCCESS);
             }else{
                wait(NULL);
                wait(NULL);
                wait(NULL);
                wait(NULL);
                printf("Terminaron todos mis hijos\n");
             }
                
            }
        
        }
    
    }


     
   
    

    return 0;
}
