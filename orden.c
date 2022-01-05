#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h> // Definición de macros
#include <sys/sem.h> // Header de diferentes funciones para crear, operar y destruir semáforos
#include <sys/wait.h>

struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.

void time5seg()
{
    sleep(2);
}

int main()
{   // Orden de los procesos: 3, 2, 1, 0
    int N = 4;
    int sems[N]; // Almacena los ids de semaforos
    int *pid_hijos = (int *) malloc(sizeof(int) * N);
    for(int i = 0 ; i < N ; i++)
    {
        sems[i] = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if(sems[i] < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
            perror("semget"); exit(1);
        }
    }

    for(int i = 0 ; i < N ; i++)
    {
        if(semctl(sems[i], 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
										  // el valor del contador (SETVAL) el valor 1.
            perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
        }
    }
    printf("Esperando el orden de los hijos...\n");
    for(int i = 0 ; i < N ; i++)
    {
        pid_hijos[i] = fork();
        if(pid_hijos[i] > 0)
        {
            continue;
        }else
        {
            pid_hijos[i] = getpid();
            if(semop(sems[i], &p, 1) < 0) { // semop realiza una operación sobre un arreglo de semaforos. Recibe la id del arreglo de semaforos,
									  // el puntero de la operación (en este caso, p, sem_wait()) y la dimensión del arreglo. Se ocupa el recurso.
			    perror("semop p"); exit(1);
		    }
            break;
        }
    }

    int pid = getpid();

    sleep(1);
    if(pid == pid_hijos[0])
    {
        if(semop(sems[3], &p, 1) < 0) { 
            perror("semop p"); exit(1);
        }
        if(semop(sems[3], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }

        if(semop(sems[2], &p, 1) < 0) { 
            perror("semop p"); exit(1);
        }
        if(semop(sems[2], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }
        if(semop(sems[1], &p, 1) < 0) { 
            perror("semop p"); exit(1);
        }
        if(semop(sems[1], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }

        printf("Soy P0 y me demoro 5 segundos...\n");
        time5seg();
        if(semop(sems[0], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }
    }else if(pid == pid_hijos[1])
    {
        if(semop(sems[3], &p, 1) < 0) { 
            perror("semop p"); exit(1);
        }
        if(semop(sems[3], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }

        if(semop(sems[2], &p, 1) < 0) { 
            perror("semop p"); exit(1);
        }
        if(semop(sems[2], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }

        printf("Soy P1 y me demoro 5 segundos...\n");
        time5seg();
        if(semop(sems[1], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }
    }else if(pid == pid_hijos[2])
    {
        if(semop(sems[3], &p, 1) < 0) {
            perror("semop p"); exit(1);
        }
        if(semop(sems[3], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }
        printf("Soy P2 y me demoro 5 segundos...\n");
        time5seg();
        if(semop(sems[2], &v, 1) < 0) {
            perror("semop v"); exit(1);
        }
    }else if(pid == pid_hijos[3])
    {
        printf("Soy P3 y me demoro 5 segundos...\n");
        time5seg();
        if(semop(sems[3], &v, 1) < 0) { // Luego se libera el recurso para que otro proceso lo pueda ocupar
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