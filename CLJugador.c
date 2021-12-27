#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int max=1024;

int main()
{
    int fd;

    char * myfifo = "/tmp/myfifo4";

    mkfifo(myfifo, 0666);

    char arr1[max], arr2[max];
    while (1)
    {

        fd = open(myfifo, O_WRONLY);

        fgets(arr2, max, stdin);

        write(fd, arr2, strlen(arr2)+1);
        close(fd);

        fd = open(myfifo, O_RDONLY);

        read(fd, arr1, sizeof(arr1));

        printf("User2: %s\n", arr1);
        close(fd);
    }
    return 0;
}