// CPP program to illustrate
// User-defined Signal Handler
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
  
// Handler for SIGINT, caused by
// Ctrl-C at keyboard



void handle_sigint(int sig)
{
    printf("Caught signal crtl C %d\n", sig);
}

void handle_sigtstp (int sig){
    printf("Caught signal crtl Z %d\n", sig);
}
  
int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    
    sleep(1);
    printf("\nesperando signal\n");
    while (1) ;
    return 0;
}