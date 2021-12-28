#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int max=1024;

int main()
{
    int fd;
    char* mypid;
    mypid = malloc(6);
    sprintf(mypid, "%d", getpid());


    char * myfifo = "/tmp/fifox2";

    mkfifo(myfifo, 0666);

    char arr1[max], arr2[max];
    char aux[max];

    while(1){
        fd = open(myfifo, O_WRONLY);
        printf("Mi pid es: %s\n",mypid);
        write(fd, mypid, strlen(mypid)+1);
        close(fd);

        fd = open(myfifo, O_RDONLY);

        read(fd, arr1, sizeof(arr1));
        sleep(3);
        printf("User2: %s\n", arr1);
        if(strncmp(arr1,"Ciclo completado",16)==0){
            break;
        }
        close(fd);
    }
    

    while (1)
    {

        fd = open(myfifo, O_WRONLY);

        //fgets(arr2, max, stdin);
        scanf("%s",arr2);

    
        
        //strcat(arr2, ",");

        //strcat(arr2, mypid);

        printf("Mi pid es: %s\n",arr2);

        write(fd, arr2, strlen(arr2)+1);
         //write(fd, arr2, strlen(arr2)+1);
        close(fd);
        //sleep(2);
        fd = open(myfifo, O_RDONLY);

        read(fd, arr1, sizeof(arr1));

        printf("User2: %s\n", arr1);
        close(fd);
    }
    return 0;
}