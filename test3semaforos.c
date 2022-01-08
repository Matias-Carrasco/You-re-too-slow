#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h> // Definición de macros
#include <sys/sem.h> // Header de diferentes funciones para crear, operar y destruir semáforos
#include <sys/wait.h>
#include <signal.h>

struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.

void time5seg()
{
    sleep(5);
}

int main()
{   // Orden de los procesos: 0, 4, 2, 1, 3
    int N = 5;
    int sems; // Almacena los ids de semaforos
    int pid_hijos[N];
    
    sems = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    
    if(sems < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
        perror("semget"); exit(1);
    }
    if(semctl(sems, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
                                        // el valor del contador (SETVAL) el valor 1.
        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }
    int pid;
    printf("Esperando el orden de los hijos...\n");
    for(int i = 0 ; i < N ; i++)
    {
        pid = fork();
        if(!pid)
        {
            break;
        }else
        {
            pid_hijos[i] = pid;
        }
    }

    if(!pid)
    {
        sleep(1);
        if(semop(sems, &p, 1) < 0) { // Espera
            perror("semop p"); exit(1);
        }
        printf("Soy %d y me demoro 5 segundos...\n", getpid());
        time5seg();
        if(semop(sems, &v, 1) < 0) { // Luego se libera el recurso para que otro proceso lo pueda ocupar
			perror("semop v"); exit(1);
		}
    }else
    {
        int status;
        do{
            wait(&status);
            N--;
        }while(N != 0);
    }

    return 0;
}