//Codigo Lab 7 BABABABA
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


sem_t sem , mutex;


void * escribirA (void *p) {
        int i;
        
        for (i= 0; i< 5; i++) {
            sem_wait(&sem);
                printf ("A"); 
                fflush(NULL);
                sleep(1);
            sem_post(&mutex);  
     
        }          
}

void * escribirB (void *p) {
    int i;
      
    for (i= 0;i< 5; i++) {
        sem_wait(&mutex);
            printf ("B");
            fflush(NULL);
            sleep(1);
        sem_post(&sem);
           
    }      
}

void main(void){
    pthread_t t1,t2;
    
    sem_init(&sem, 1, 1); //esto es importante para ver que semaforo se ejecuta primero 1 tiene prioridad y los con 0 son sin prioridad.
    sem_init(&mutex, 0, 0);
    
    pthread_create(&t1 ,NULL, (void * ) escribirA, (void * ) NULL);
    pthread_create(&t2 ,NULL, (void * ) escribirB, (void * ) NULL); 
   

     pthread_join(t1, NULL);
    pthread_join(t2, NULL);    
    // Ejecución del hilo

    
}