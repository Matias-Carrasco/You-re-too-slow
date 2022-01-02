
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

int max = 1024;

int matrix[12][12];
int x[144], y[144];
int actual = 0;
int puntaje[4];

typedef enum
{
    false = 0,
    true = 1
} bool;

int iniciarmatrix(int tamanoMatrix)
{
    int i, j;
    for (i = 0; i < tamanoMatrix; i++)
    {
        for (j = 0; j < tamanoMatrix; j++)
        {
            matrix[i][j] = i;
        }
    }
}

int main()
{
    //Variables 
    int fd1;               //File Descriptor
    int i, j, k;           //iteradores
    int tamanoMatrix = 12; //Tamaño de la matrix 
    pid_t hijo; 



    iniciarmatrix(tamanoMatrix);
    //imprimir los valores de x, y
    for (i = 0; i < 144; i++)
    {
        x[i] = 99;
        y[i] = 99;

    }

    char *myfifo = "/tmp/slow";

    mkfifo(myfifo, 0666);

    char str1[max], str2[max];
    
    hijo = fork();
   

    if(hijo == 0)
    {
        while (1)
        {

          
           
            //Variables locales (por fork)
            bool bandera = false;
            bool bandera2 = false;
            int posidecoma, auxX, auxY;
            int cont = 0;
            char aux[1024] = "\n"; //dejo en 1024 por que se bugea con max
            char xt[3] = "";
            char yt[3] = "";

   

            fd1 = open(myfifo, O_RDONLY);
            read(fd1, str1, max);

       
            printf("User1: %s\n", str1);
            close(fd1);

            fd1 = open(myfifo, O_WRONLY);

            fflush(stdin); //Borrar cache
            strncpy(str2, "", 0);

            for (i = 0; i < strlen(str1); i++)
            {
                if ((str1[i]) == ',')
                {
                    posidecoma = i;
                }
            }

            for (i = 0; i < posidecoma; i++)
            {
                xt[i] = str1[i];
            }

            for (i = posidecoma + 1; strlen(str1) - 1; i++)
            {

                if (str1[i] == 10)
                {
                    break;
                }
                yt[cont] = str1[i];
                cont++;
            }

            //atoi = Convertir String en entero
            auxX = atoi(xt);
            auxY = atoi(yt);


           

            for (i = 0; i < actual + 1; i++)
            {
                if ((x[i] == auxX && y[i] == auxY))
                {
                    bandera = true;
                    break;
                }
            }

            if (auxX > tamanoMatrix - 1 || auxY > tamanoMatrix - 1 || auxX < 0 || auxY < 0)
            {
                bandera = true;
            }

            if (bandera == true)
            {
                strncpy(str2, "Error, posicion no valida", 26);
            }
            else
            {
                x[actual] = auxX;
                y[actual] = auxY;

                for (i = 0; i < tamanoMatrix; i++)
                {
                    for (j = 0; j < tamanoMatrix; j++)
                    {
                        if (auxX == i && auxY == j)
                        {
                            puntaje[0] = puntaje[0] + matrix[i][j];
                        }
                    }
                }

                for (i = 0; i < tamanoMatrix; i++)
                {
                    for (j = 0; j < tamanoMatrix; j++)
                    {
                        for (k = 0; k < actual + 1; k++)
                        {
                            if (x[k] == i && y[k] == j)
                            {
                                bandera2 = true;
                            }
                        }

                        if (bandera2 == true)
                        {

                            strcat(aux, "[X] ");
                            bandera2 = false;
                        }
                        else
                        {
                            strcat(aux, "[]  ");
                        }
                    }
                    strcat(aux, "\n");
                }
                strcat(aux, "Puntaje actual: ");
                char punt[4];
                sprintf(punt, "%d", puntaje[0]);
                strcat(aux, punt);

                strncpy(str2, aux, sizeof(aux));
                actual++;
            }

            write(fd1, str2, strlen(str2) + 1);
            close(fd1);

            
            
           
        }
     
    }else{
        wait(NULL);
        printf("\n");
        printf("\n Termino un turno, me pasaron tal cosa y yo tengo que devolver tal cosa\n");
    }


     
   
    

    return 0;
}
