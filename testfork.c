
// C program to implement one side of FIFO
// This side reads first, then reads

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

 
#include <semaphore.h> //semaforo
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> //wait pid

#define SEM_NAME_1 "/sem_1"
#define SEM_NAME_2 "/sem_2"


int main()
{

    sem_t *sem1 = NULL, *sem2 = NULL, *sem3 = NULL, *sem4 = NULL;

    /* If O_CREAT and O_EXCL are here then if the semaphore exists it will give us an error */
    sem1 = sem_open(SEM_NAME_1, O_CREAT | O_EXCL, 0, 0);
    sem2 = sem_open(SEM_NAME_2, O_CREAT | O_EXCL, 0, 0);
    
    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);
    

    


    int fd1; 
    pid_t hijo1; 
    pid_t hijo2; 
    pid_t hijo3; 
    pid_t hijo4; 

    char *myfifo = "/tmp/fifox2";

    mkfifo(myfifo, 0666);
    int max = 1024;
    char str1[max], str2[max];


    hijo1=fork();


    if(hijo1 == 0)
    {
        int pidjugador=0;
        puts("hijo 1\n");
        while(1){
            if(pidjugador!=0){
                puts("Encontre mi asociado\n");
                break;
            }
            
            fd1 = open(myfifo, O_RDONLY);
            
            read(fd1, str1, max);
            close(fd1);
            if(strlen(str1)>0){
                pidjugador = atoi(str1); 
                printf("Soy el hijo con id: %d y me fue asignado el jugador: %d\n",getpid(),pidjugador);
                //sleep(2);
            }else{
                puts("Nadie ingresa aun..\n");
            }

            
            fd1 = open(myfifo, O_WRONLY);
            write(fd1, "Ciclo completado\n",16);
            close(fd1);
        }
        sem_post(sem1);
        sem_wait(sem2);

        sem_close(sem1);
        sem_close(sem2);
        
        while(1){
            fd1 = open(myfifo, O_RDONLY);
            read(fd1, str1, max);
            //pidjugador = atoi(str1);
            close(fd1);
            puts("Estoy dentro Hijo 1!");
            //sleep(2);
            if( strncmp(str1,"coxis",5)==0){
                puts("A aparecido un coxis");
                break;
            }

            
            fd1 = open(myfifo, O_WRONLY);
            write(fd1, "AAAAAAAA",16);
            close(fd1);
        }

        puts("Se cierra el hijo 1\n");

 


        exit(EXIT_SUCCESS);
    }else{
          hijo2=fork();
            if(hijo2==0){
                sem_wait(sem1);
                int pidjugador=0;
                puts("hijo 2\n");
                while(1){
                    if(pidjugador!=0){
                        puts("Encontre mi asociado\n");
                        break;
                    }
                    
                    fd1 = open(myfifo, O_RDONLY);
                    
                    read(fd1, str1, max);
                    close(fd1);
                    if(strlen(str1)>0){
                        pidjugador = atoi(str1); 
                        printf("Soy el hijo con id: %d y me fue asignado el jugador: %d\n",getpid(),pidjugador);
                        //sleep(2);
                    }else{
                        puts("Nadie ingresa aun..\n");
                    }

                    
                    fd1 = open(myfifo, O_WRONLY);
                    write(fd1, "Ciclo completado\n",16);
                    close(fd1);
                } 

                sem_post(sem2);

                sem_close(sem1);
                sem_close(sem2);



                while(1){
                    fd1 = open(myfifo, O_RDONLY);
                    read(fd1, str1, max);
                    //pidjugador = atoi(str1);
                    close(fd1);
                    puts("Estoy dentro Hijo 2!");
                    if( strncmp(str1,"coxis",5)==0){
                        puts("A aparecido un coxis");
                        break;
                    }
                    
                    fd1 = open(myfifo, O_WRONLY);
                    write(fd1, "BBBBBBBBB",16);
                    close(fd1);
                    
                }

                puts("Se cierra el hijo 2\n");
                
             


                exit(EXIT_SUCCESS);
              

            }else{
                wait(NULL);
                wait(NULL);
                
            
                puts("Terminaron todos mis hijos\n");
            }   
    }


     
   
    

    return 0;
}
