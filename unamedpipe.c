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
#include <sys/sem.h> // Header de diferentes funciones para crear, operar y destruir semáforos
#include <semaphore.h>
#include <sys/shm.h>

//Memoria compartida
#include <sys/stat.h>
#include <sys/mman.h>

//sincronisacion de fd
#include <poll.h>
#include <sys/select.h>


struct sembuf p = {0, -1, SEM_UNDO}; // Estrutura que define la operación atomica sem_wait(). 0 significa que se va a operar en el indice 0 del arreglo
									 // de semáforos. -1 significa que se va a disminuir el contador del semáforo, SEM_UNDO significa que la operación
									 // aplicada será mientras viva el proceso, es decir, si el proceso muere antes de liberar el recurso o sección
									 // critica, automáticamente se liberar dicho recurso, esto para evitar deadlock (Ver clase 7)
struct sembuf v = {0, 1, SEM_UNDO}; // Estructura que define la operación atomica sem_post(). Mismo caso que la operación anterior, con la diferencia
									// que 1 significa que se va a aumentar el contador del semáforo.


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

void time5seg()
{
    sleep(5);
}

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
    //pid de proceso hijos
    int pid_hijos[jugadores];

    //semaforos
    int sems;

    sems = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if(sems < 0) { // Si no se pudo crear el arreglo, retorna un valor negativo
        perror("semget"); exit(1);
    }
    if(semctl(sems, 0, SETVAL, 1) < 0) { // semctl inicializa un semaforo, para un arreglo de semaforos (semid), en su posición 0, se va a inicializar
                                        // el valor del contador (SETVAL) el valor 1.
        perror("semctl"); exit(1); // Si falla, retorna un valor negativo.
    }


    //para el fifo
    char *myfifo = "/tmp/fifo";
    mkfifo(myfifo, 0666);
    int fd; //filedescriptor del fifo

            
    

    //matriz oculta
    int (*oculta)[tamanomatrix] = (int(*)[tamanomatrix]) mmap(NULL, sizeof(int)*tamanomatrix*tamanomatrix, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for(i=0 ; i<tamanomatrix ; i++){
        for(j=0 ; j<tamanomatrix ; j++){
            oculta[i][j] = 0;
            //printf("[%d]",oculta[i][j]);
        }
        //printf("\n");
    }

    int pidjug[4];
    int pipes[jugadores * 2][2];
    int *childread;
    int *childwrite;
    int *dadread;
    int *dadwrite;

    for (i = 0; i < jugadores * 2; i++)
    {
        pipe(pipes[i]);
    }
    
    int hijo;
    //fork
    for (i = 0; i < jugadores; i++)
    {

        hijo=fork();
        if (hijo < 0)
        {
            

            printf("Error al crear hijo %d", i);
            exit(EXIT_FAILURE);
        }
        else if (hijo == 0)
        {
            pid_hijos[i] = hijo;
            puts("Esperando a que ingrese un jugador");
            sleep(1);
            if(semop(sems, &p, 1) < 0) { // Espera
                perror("semop p"); exit(1);
            }
            //recivir el pid del hijo
            char msj[20];
            fd = open(myfifo, O_RDONLY);
            fsync(fd);
            read(fd,msj,20);
            
            
            close(fd);
            int pidj = atoi(msj);
            pidjug[i]=pidj;

            //mandar el pid del padre
            fd = open(myfifo, O_WRONLY);    
            char res[20];
            int pid = getpid();
            sprintf(res,"%d",getppid()); 
            write(fd,res,20);
            fsync(fd);
            close(fd);

            printf("[Hijo %d] a ingresado el jugador %d\n",i,pidj);
            char resp[1024];
         

            childread = &pipes[2 * i][0];
            childwrite = &pipes[2 * i][1];
            dadread = &pipes[(2 * i) + 1][0];
            dadwrite = &pipes[(2 * i) + 1][1];
         

            if(semop(sems, &v, 1) < 0) { // Luego se libera el recurso para que otro proceso lo pueda ocupar
                perror("semop v"); exit(1);
            }
            
            

            break;
        }
    }

    //Logica de los pipes

        if (hijo == 0 )
        {
            //Codigo del hijo
            sleep(1);
    
          
            int puntajeacumulado=0;
            int puntajeagregado;
            
            close(*childread);
            close(*dadwrite);
            while(1){

                char mensaje[10];
                char respuesta[1024];
            
                //desmenusa las cordenadas en enteros (sin modificar el mensaje)
                //verificar que sean correctas (si la matriz es de 4,4 que sean valores entre 0,3) y (que La coordenada ya este siendo utilizada)
                //le manda al padre las cordenadas si son corrertas si no manda un mensaje de error de vuelta al jugador.
                bool valid=false;
                int auxX,auxY;
                char xt[3];
                char yt[3];
                bool formato;
            
                
                char conexion[]="Ganaste el turno";
                char conexionfail[]="Turno perdido";
                int cont,posidecoma;

                do{
                    cont = 0;
                    posidecoma = 0;
                    puts("Esperando coordenadas: ");

                    fd = open(myfifo, O_RDONLY);
                    read(fd, mensaje, 1024);
                    fsync(fd);
                    close(fd);

                    if(strcmp(mensaje,"MG")==0){
                        puts("Juego terminado!\n");
                        exit(0);
                    }


                    fd = open(myfifo, O_WRONLY);
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
                        //pasar al padre
                        printf("Proceso hijo:  Las coordenadas [%d,%d] son validas, Enviando.....\n",auxX,auxY);
                        ssize_t result;
                        write(*childwrite, &auxX, sizeof(int)); 
                        
                        
                        write(*childwrite, &auxY, sizeof(int)); 
                        

                        valid=true;
                    }else{
                        //mandar mensaje de error al jugador y volver a preguntar por coords
                        char error[10] = "error";
                        printf("Proceso hijo: ERROR[1523] Las coordenadas [%d,%d] NO son validas\n",auxX,auxY);
                        write(fd, error, 10);
                        fsync(fd);
                        close(fd);

                    }

                }while(valid==false);
                
            
                read(*dadread, &puntajeagregado, sizeof(int));
            

                puntajeacumulado+=puntajeagregado;
                
                printf("Proceso hijo: Puntaje actual: %d\n",puntajeacumulado);
                explicit_bzero(respuesta, 1024);
                int con=0;
                for (i = 0; i < tamanomatrix; i++)
                {
                    for (j = 0; j < tamanomatrix; j++)
                    {

                        if (oculta[i][j]==1)
                        {
                            con++;
                            strcat(respuesta, "[X] ");
                        }
                        else
                        {
                            strcat(respuesta, "[O] ");
                        }
                    }
                    strcat(respuesta, "\n");
                }
                strcat(respuesta, "\n");
                char punt[4];
                sprintf(punt, "%d", puntajeacumulado);
                if(con==(tamanomatrix*tamanomatrix)){
                    
                    //el juego a terminado, no mas posiciones
                    strcat(respuesta, "Juego Finalizado, Tu puntaje obtenido es:  ");
                    strcat(respuesta, punt);
                    strcat(respuesta, "\n");
                    write(fd, respuesta, 1024);

                    kill(pidjug[0],SIGKILL);
                    exit(0);


                }

                strcat(respuesta, "Puntaje actual: ");
                strcat(respuesta, punt);

                write(fd, respuesta, 1024);
                close(fd);

            }

        }
        else if (hijo > 0)
    {
        //Codigo del padre
        int auxX,auxY;
        int numero;
        int puntaje;
        int puntajeacumulado;
       

        int real[tamanomatrix][tamanomatrix];

        matriz(real);

        
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
                puntajeacumulado += puntaje;
                

                write(pipes[(2 * i) + 1][1], &puntaje, sizeof(int));

               
            }
            if(puntajeacumulado >= 20){
                wait(NULL);
                exit(0);
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