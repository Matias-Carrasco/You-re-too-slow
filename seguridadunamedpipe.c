#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//Rrrores
#include <fcntl.h>
#include <errno.h>

//Fork
#include <sys/wait.h>


//Semaforos
#include <semaphore.h>
#include <sys/shm.h>

//Memoria compartida
#include <sys/stat.h>
#include <sys/mman.h>




int main()
{
    int i,j;
    pid_t hijo;
    int fork_count;
    int filas = 12;
    int columnas = 12;

    char* oculta = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
   

    puts("Cuantos jugadores quiere?");
    scanf("%d", &fork_count);
    
    int pipes[fork_count* 2][2];
    int *childread;
    int *childwrite;
    int *dadread;
    int *dadwrite;


    for(i = 0; i < fork_count*2; i++){
        pipe(pipes[i]);
    }

    //Se crea el fork
    for (i = 0; i < fork_count; i++)
    {

        hijo = fork();
        if (hijo < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (hijo == 0)
        {
           
            childread =&pipes[2*i][0];
            childwrite = &pipes[2*i][1];
            dadread = &pipes[(2*i)+1][0];
            dadwrite = &pipes[(2*i)+1][1];
       

            break;
        }
    }


    //Logica de los pipes


    if (hijo == 0)
    {
        int numero=getpid();
        ssize_t result;
        printf("Proceso hijo: Numero: %d\n",numero);
        close(*childread);
        //cambiando valor de shared a 
  
        strcat(oculta, "[0] [0] [0]");
        printf("Proceso hijo: Shared value: %s\n",oculta);
        write(*childwrite,&numero,sizeof(int));
        close(*dadwrite);
        read(*dadread,&numero,sizeof(int));

        printf("**** Proceso hijo:  Numero recivido igual a: %d\n",numero);

        //exit(EXIT_SUCCESS);
        //exit(0);
    }
    else if (hijo > 0)
    {   
        int numero;
        ssize_t result;
     
        for (i = 0; i < fork_count; i++)
        {
            close(pipes[2*i][1]);
            read(pipes[2*i][0],&numero,sizeof(int));
            printf("Proceso padre: Shared value: %s\n",oculta);
            printf("Proceso padre: Numero: %d\n",numero);
            numero-=1000;
            printf("Proceso padre: Numero -1000: %d\n",numero);

            close(pipes[(2*i)+1][0]);
            write(pipes[(2*i)+1][1],&numero,sizeof(int));
        }

        printf("Proceso padre: He finalizado\n");
       
    }
}