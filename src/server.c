#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]){
    const uint16_t INCOMMING_CONNECTION_QUEUE_MAX = 10;
    uint16_t port;
    //parse input
    if (argc > 2){
        printf("USAGE: ./server [port(default=8000)]");
        exit(EXIT_FAILURE);
    }else if (argc == 2){
        port = atoi(argv[1]);
    }else{
        port = 8000;
    }
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
        printf("Error creating socket : %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    //bind
    struct sockaddr_in sockaddr_in_server;
    sockaddr_in_server.sin_family = AF_INET;
    sockaddr_in_server.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr_in_server.sin_port = htons(port);
    socklen_t sockaddr_in_server_length = sizeof(sockaddr_in_server);
    
    int result = bind(server_fd, (struct sockaddr *)&sockaddr_in_server, sockaddr_in_server_length);
    if (result == -1){
        printf("Error binding to address:%s\n",strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    result = listen(server_fd, INCOMMING_CONNECTION_QUEUE_MAX);
    if ( result == -1 ){
        printf("Error listen():%s\n",strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %u\n",port);

}