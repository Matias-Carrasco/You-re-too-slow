#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include <sys/wait.h> //WAIT system function

#include <sys/types.h> //llamadas del sistema

int players;
int matrix[12][12];

void printmatrix();
void clearmatrix();
void menu();
void matriz(int tamanoMatrix, int cantRandom);


int main()
{
    menu();

    int contador=1;
    pid_t hijo;
    hijo = fork();
  
while(true){
   
    //error handler
    if(hijo < 0) {
        perror("Error al crear proceso\n");
        exit(1);
    }
    if(hijo == 0) {
        //codigo lo que tiene que hacer el hijo
        int id = getpid();
        printf("\n%d.Soy el hijo con el id: %d ",contador,id);


        if(contador >= 4){
            printf("\n");
            exit(0);
            
        }
      
        

    }else{
        if(contador == 1){
        printf("%d.Soy el padre",contador);
        wait(NULL);
        }
       
     
        
    }


    sleep(1);
    contador++;

    if(contador == 5){
        hijo = fork();
        contador = 1;
    }


}


return 0;
}

void menu(){
    printf("Bienvenido a You’re too slow!\n");

    printf("Selecione el numero de jugadores: ");
    scanf("%d",&players);

    switch(players)
    {
        case 2:
        printf("Esperando a 2 jugadores\n");
        matriz(8,16); 
        break;

        case 3:
        printf("Esperando a 3 jugadores\n");
        matriz(10,25);
        break;

        case 4:
        printf("Esperando a 4 jugadores\n");
        matriz(12,36);
        break;

        default:
        printf("El valor ingresado no es correcto\n");
        menu();
    }
    
 
}

// (x,y) x es el tama;o de la matriz y 
void matriz(int tamanoMatrix, int cantRandom){

srand(time(NULL));
matrix[tamanoMatrix][tamanoMatrix];
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  int ver=0;
int numerosOr[50];
int numerosDes[50];
bool usado[50];



for(i=0;i<50;i++){
    numerosOr[i]=i+1;
}

for(i=0;i<50;i++){
    usado[i]=false;
}

int index = 0;

for(i=0;i<50;i++){
   do{
       index = rand()%50;
   }while(usado[index]);
   numerosDes[i]=numerosOr[index];
   usado[index]=true;
}

/*
printf("\nProcedo a imprimir el vector\n");

for(i=0;i<cantRandom;i++){
    printf("[%d]",numerosDes[i]);
}
*/

//Llenar matriz con 0;
clearmatrix();



 int count=0; 
 int rx=0;
 int ry=0;

 int coordX[cantRandom];
 int coordY[cantRandom];
 int contadorCor=0;


for (i=0;i<cantRandom;i++){
    coordX[i]=0;
    coordY[i]=0;
}


 int coordXRan[cantRandom];
 int coordYRan[cantRandom];

bool bandera = true;


  for (i=0;i<cantRandom;i++){
     coordXRan[i]=rand()%tamanoMatrix;
     coordYRan[i]=rand()%tamanoMatrix;
     
      //printf("[%d,%d]",coordXRan[i],coordYRan[i]);
    for(j=0;j<cantRandom;j++){
        if( (coordX[j] == coordXRan[i]) && (coordY[j] == coordYRan[i]) ){
            i--;
            bandera = false;
            break;
            printf("Numero repetido \n");
        }
    }

    if(bandera==true){
        coordX[contadorCor]=coordXRan[i];
        coordY[contadorCor]=coordYRan[i];
        //printf("%d encontrado\n",contadorCor+1);
        contadorCor++;
        
    }

    bandera = true;
  }


/* prueba para ver los que tan el random de las coordenadas
printf("\nProcedo a imprimir el vector coordenadas\n");
for(i=0;i<cantRandom;i++){
    printf("[%d,%d]",coordX[i],coordY[i]);
}
*/


//Reparte los numeros random en la matrix        
  for (i=0;i<cantRandom;i++){
     matrix[coordX[i]][coordY[i]]=numerosDes[i];
      
}

    printmatrix(); 
}

void printmatrix(){
    int tamanoMatrix, i, j;

    switch(players)
    {
        case 2:
        tamanoMatrix = 8;
        break;

        case 3:
        tamanoMatrix = 10;
        break;

        case 4:
        tamanoMatrix = 12;
        break;

    }

  
    int contador=0;
    printf("\nProcedo a imprimir la matriz\n");
    for (i=0;i<tamanoMatrix;i++){
        for (j=0;j<tamanoMatrix;j++){
            if(matrix[i][j]<10){
                printf("[%d]  ",matrix[i][j]);
            }else{
            printf("[%d] ",matrix[i][j]);
        }
        if(matrix[i][j] != 0){
            contador++;
        }
    }
    printf("\n");

  }
   printf("Cantidad de numeros no repetidos: %d\n",contador);

}

void clearmatrix(){
    int tamanoMatrix, i, j;

    switch(players)
    {
        case 2:
        tamanoMatrix = 8;
        break;

        case 3:
        tamanoMatrix = 10;
        break;

        case 4:
        tamanoMatrix = 12;
        break;

    }
    
    for (i=0;i<tamanoMatrix;i++){
        for (j=0;j<tamanoMatrix;j++){
        matrix[i][j]=0;
        }
    }

}