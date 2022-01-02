#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>



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

//sincronisacion de fd
#include <poll.h>
#include <sys/select.h>

//Variables globales
int jugadores;
int tamanomatrix;
int numerosrandom;
int buffsize = 100000;
//int turno = 0;

void juego();
void iniciar();
void matriz(int real[tamanomatrix][tamanomatrix]);
bool validarcoord(int auxX,int auxY,int oculta[tamanomatrix][tamanomatrix]);

int main()
{
    iniciar();
}

void iniciar()
{

    printf("Bienvenido a You re too slow!\n");

    printf("Selecione el numero de jugadores: ");
    //scanf("%d", &jugadores);
    jugadores=1;

    switch (jugadores)
    {
    case 1:
        printf("Esperando a 2 jugadores\n");
        tamanomatrix = 4;
        numerosrandom = 4;
        break;
    case 2:
        printf("Esperando a 2 jugadores\n");
        tamanomatrix = 8;
        numerosrandom = 16;
        break;

    case 3:
        printf("Esperando a 3 jugadores\n");

        tamanomatrix = 10;
        numerosrandom = 25;
        break;

    case 4:
        printf("Esperando a 4 jugadores\n");
        tamanomatrix = 12;
        numerosrandom = 36;
        break;

    default:
        printf("El valor ingresado no es correcto\n");
        iniciar();
    }
    juego();
}

void juego()
{

    int i, j;
    pid_t hijo;
    sem_t *sem[2];

    struct timeval tv;
    fd_set readfds;

    

    sem[0] = sem_open("pSem0", O_CREAT | O_EXCL, 0644, 0);
    sem[1] = sem_open("pSem1", O_CREAT | O_EXCL, 0644, 0);
    sem_unlink ("pSem0");
    sem_unlink ("pSem1");
    
    //matriz oculta
    int (*oculta)[tamanomatrix] = (int(*)[tamanomatrix]) mmap(NULL, sizeof(int)*tamanomatrix*tamanomatrix, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for(i=0 ; i<tamanomatrix ; i++){
        for(j=0 ; j<tamanomatrix ; j++){
            oculta[i][j] = 0;
            //printf("[%d]",oculta[i][j]);
        }
        //printf("\n");
    }

    int pipes[jugadores * 2][2];
    int *childread;
    int *childwrite;
    int *dadread;
    int *dadwrite;

    for (i = 0; i < jugadores * 2; i++)
    {
        pipe(pipes[i]);
    }
    
    //fork
    for (i = 0; i < jugadores; i++)
    {

        hijo = fork();
        if (hijo < 0)
        {

            printf("Error al crear hijo %d", i);
            exit(EXIT_FAILURE);
        }
        else if (hijo == 0)
        {

            childread = &pipes[2 * i][0];
            childwrite = &pipes[2 * i][1];
            dadread = &pipes[(2 * i) + 1][0];
            dadwrite = &pipes[(2 * i) + 1][1];

            break;
        }
    }

     tv.tv_sec = 0,1;
     tv.tv_usec = 0;
    //Logica de los pipes
    if (hijo == 0)
    {
        //Codigo del hijo
        FD_ZERO(&readfds);
        FD_SET(*dadread, &readfds);
        int sret;



        sem_wait(sem[0]);
        char mensaje[10];
        
        int puntajeacumulado=0;
        int puntajeagregado;
        ssize_t result;
        close(*childread);
        close(*dadwrite);
        while(1){
            

            



           

            //desmenusa las cordenadas en enteros (sin modificar el mensaje)
            //verificar que sean correctas (si la matriz es de 4,4 que sean valores entre 0,3) y (que La coordenada ya este siendo utilizada)
            //le manda al padre las cordenadas si son corrertas si no manda un mensaje de error de vuelta al jugador.
            bool valid=false;
            int auxX,auxY;
            char xt[3];
            char yt[3];
            bool formato;
            
            int cont,posidecoma;

            do{
                cont = 0;
                posidecoma = 0;

            
                printf("Proceso hijo: Ingrese unas coordenadas: \n");
                  
                scanf("%s",mensaje);

                for (i = 0; i < strlen(mensaje); i++)
                {
                    if ((mensaje[i]) == ',')
                    {
                        posidecoma = i;
                        break;
                    }
                }

                for (i = 0; i < posidecoma; i++)
                {
                    xt[i] = mensaje[i];
                }

                for (i = posidecoma + 1; strlen(mensaje) - 1; i++)
                {

                    if (mensaje[i] == 10)
                    {
                        break;
                    }
                    yt[cont] = mensaje[i];
                    cont++;
                }
        
                auxX = atoi(xt);
                auxY = atoi(yt);
     

                if(validarcoord(auxX,auxY, oculta)==true){ 
                    
                    printf("Proceso hijo:  Las coordenadas [%d,%d] son validas, Enviando.....\n",auxX,auxY);
                    write(*childwrite, &auxX, sizeof(int)); 
                    write(*childwrite, &auxY, sizeof(int)); 
                    valid=true;
                }else{
                    //mandar mensaje de error al jugador y volver a preguntar por coords
                    printf("Proceso hijo: ERROR[1523] Las coordenadas [%d,%d] NO son validas\n",auxX,auxY);
                }

            }while(valid==false);
            
           
            result = read(*dadread, &puntajeagregado, sizeof(int));
           

            puntajeacumulado+=puntajeagregado;
      
            printf("Proceso hijo: Puntaje actual: %d\n",puntajeacumulado);

            if(puntajeacumulado >= 20){
                printf("Felicidades haz ganado! puntaje total %d\n",puntajeacumulado);
                exit(1);
            }
        }
        


    }
    else if (hijo > 0)
    {
        //Codigo del padre
        int auxX,auxY;
        int numero;
        int puntaje;
        int puntajeacumulado;
        ssize_t result;

        int real[tamanomatrix][tamanomatrix];

        matriz(real);

         

        

        sem_post(sem[0]);
        close(pipes[2 * i][1]);
        close(pipes[(2 * i) + 1][0]);
        while(1){

            auxX = -1;
            auxY = -1;
            
            

            for (i = 0; i < jugadores; i++)
            {
                
                read(pipes[2 * i][0], &numero, sizeof(int));
                auxX=numero;
                read(pipes[2 * i][0], &numero, sizeof(int));
                auxY=numero;
                //manda el valor de la matriz real al hijo y modifica
            
                
                printf("Proceso padre: recivi las coordenadas: [%d,%d]\n",auxX,auxY);


                oculta[auxX][auxY]=1;
                puntaje = real[auxX][auxY];
            

                result = write(pipes[(2 * i) + 1][1], &puntaje, sizeof(int));

               
            }
            if(puntajeacumulado >= 20){
                wait(NULL);
                break;
            }
          
        }

     

        printf("Proceso padre: He finalizado\n");
    }
}

// (x,y) x es el tama;o de la matriz y
void matriz(int real[tamanomatrix][tamanomatrix])
{

    srand(time(NULL));
    
    int coordX[numerosrandom];
    int coordY[numerosrandom];

    int coordXRan[numerosrandom];
    int coordYRan[numerosrandom];

    int i = 0, j = 0, k = 0, l = 0, index = 0, ver = 0, count = 0, rx = 0, ry = 0, contadorCor = 0;
    int numerosOr[50];
    int numerosDes[50];
    bool usado[50];
    bool bandera = true;

    for (i = 0; i < 50; i++)
    {
        numerosOr[i] = i + 1;
    }

    for (i = 0; i < 50; i++)
    {
        usado[i] = false;
    }

    for (i = 0; i < 50; i++)
    {
        do
        {
            index = rand() % 50;
        } while (usado[index]);
        numerosDes[i] = numerosOr[index];
        usado[index] = true;
    }

    for (i = 0; i < tamanomatrix; i++)
    {
        for (j = 0; j < tamanomatrix; j++)
        {
            real[i][j] = 0;
        }
    }

    for (i = 0; i < numerosrandom; i++)
    {
        coordX[i] = 0;
        coordY[i] = 0;
    }

    for (i = 0; i < numerosrandom; i++)
    {
        coordXRan[i] = rand() % tamanomatrix;
        coordYRan[i] = rand() % tamanomatrix;

        for (j = 0; j < numerosrandom; j++)
        {
            if ((coordX[j] == coordXRan[i]) && (coordY[j] == coordYRan[i]))
            {
                i--;
                bandera = false;
                break;
                printf("Numero repetido \n");
            }
        }

        if (bandera == true)
        {
            coordX[contadorCor] = coordXRan[i];
            coordY[contadorCor] = coordYRan[i];

            contadorCor++;
        }

        bandera = true;
    }

    //Reparte los numeros random en la matrix
    for (i = 0; i < numerosrandom; i++)
    {
        real[coordX[i]][coordY[i]] = numerosDes[i];
    }

    int contador = 0;
    printf("\nProcedo a imprimir la matriz\n");
    for (i = 0; i < tamanomatrix; i++)
    {
        for (j = 0; j < tamanomatrix; j++)
        {
            if (real[i][j] < 10)
            {
                printf("[%d]  ",real[i][j]);
            }
            else
            {
                printf("[%d] ", real[i][j]);
            }
            if (real[i][j] != 0)
            {
                contador++;
            }
        }
        printf("\n");
    }
    printf("Cantidad de numeros no repetidos: %d\n", contador);

   
}

//Retorna true cuando las coordenadas son validas y false cuando las coordenadas son invalidas
bool validarcoord(int auxX,int auxY,int oculta[tamanomatrix][tamanomatrix]){
        printf("Validacion de coords\n");
        printf("X: %d, Y: %d\n",auxX,auxY);
   
    //Comprobar si la coordenada esta ya en la matriz oculta
    if(oculta[auxX][auxY] == 1) return false;
    

    //Comprobacion de los limites
    if (auxX > tamanomatrix - 1 || auxY > tamanomatrix - 1 || auxX < 0 || auxY < 0)
    {
        return false;
    }


    return true;
}