#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>


int main()
{
    int i;
    pid_t pid;
    int fork_count;
    int jugadores[4];
    int jugadorActual=0;

    int fd1; 
    char *myfifo = "/tmp/myfifo4";
    mkfifo(myfifo, 0666);



    puts("Cuantos jugadores quiere?");
    scanf("%d", &fork_count);
  
    
    for (i = 0; i < fork_count; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            break;
        }
    }
    if (pid == 0)
    {

        int pidjugador;
        int pidaux;

        char aux[1024] = "\n";
        char xt[3] = "";
        char yt[3] = "";


        int max = 1024;
        char str1[max], str2[max];

        while (1)
        {
            fd1 = open(myfifo, O_RDONLY);
            pidaux = 0;
            if(pidjugador == 0){ //ENTREAR SOLO LA PRIMERA VEZ
                read(fd1, str1, max);
                pidjugador = atoi(str1); 
                printf("Soy el hijo con id: %d y me fue asignado el jugador: %d\n",getpid(),pidjugador);
                close(fd1);
            }else{
                read(fd1, str1, max);
                //hacemos la coma
                pidaux=atoi(str1);
                if(pidjugador==pidaux)
                {
                    printf("Soy el hijo con id: %d y se me esta siendo comparado con %d\n",getpid(),pidjugador);
                }else{
                    printf("Soy el hijo con id: %d y no me corresponde el jugador %d\n",getpid(),pidaux);
                }
                close(fd1);
            }

            //respuesta test
            fd1 = open(myfifo, O_WRONLY);

            write(fd1, "Ciclo completado",16);
            close(fd1);

            sleep(1);

        }

        printf("Soy el hijo %d\n",i);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        //parent process
        while (pid = waitpid(-1, NULL, 0))
        {
            if (errno == ECHILD)
            {
                break;
            }
        }
        printf("Terminaron todos mis hijos\n");

        exit(0);
    }
}