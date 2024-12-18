#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigintHandler(int signo);

int main(){
    signal(SIGINT, sigintHandler);
    for(;;);

    return 0;
}

void sigintHandler(int signo){
    if( signo == SIGINT ){
        printf("Shutting down...\n");
        exit(EXIT_FAILURE);
    }
}