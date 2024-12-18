#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define SERVER_DEFAULT_PORT 8000
#define INCOMING_CONNECTIONS_QUEUE_MAX 10

void sigintHandler(int signo);
void terminate(int exit_code);
void parseInput(int argc, char *argv[]);
int serverInit(uint16_t port);

int server_fd;
uint16_t port;

int main(int argc, char *argv[]){
    //register SIGINT handler
    signal(SIGINT, sigintHandler);
    //parse input
    parseInput(argc, argv);
    //initialize server
    server_fd = serverInit(port);
    printf("Listening on port %u\n", port);

    for(;;);

    terminate(EXIT_SUCCESS);
}

int serverInit(uint16_t port){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( server_fd == -1){
        printf("Error creating socket: %s\n",strerror(errno));
        terminate(EXIT_FAILURE);
    }

    //bind to socket
    struct sockaddr_in server_sockaddr_in;
    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(port);

    if( -1 == bind(server_fd, (struct sockaddr *)&server_sockaddr_in, sizeof(server_sockaddr_in)) ){
        printf("Error binding to port %u: %s\n",port, strerror(errno));
        terminate(EXIT_FAILURE);
    }

    //listen for incoming connections
    if ( -1 == listen(server_fd, INCOMING_CONNECTIONS_QUEUE_MAX) ){
        printf("Error trying to listen on port %u: %s\n", port, strerror(errno));
        terminate(EXIT_FAILURE);
    }
    return server_fd;
}

void parseInput(int argc, char *argv[]){
    if( argc > 2 ){
        printf("USAGE: server [PORT(default=8000)]\n");
        exit(EXIT_SUCCESS);
    }else if ( argc == 2 ){
        port = atoi(argv[1]);
    }else{
        port = SERVER_DEFAULT_PORT;
    }
}

void terminate(int exit_code){
    printf("Shutting down.\n");
    close(server_fd);
    exit(exit_code);
}

void sigintHandler(int signo){
    if( signo == SIGINT ){
        terminate(EXIT_SUCCESS);
        exit(EXIT_FAILURE);
    }
}