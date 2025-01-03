#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define RECEIVE_BUFFER_SIZE 1024

uint16_t parseInput(int argc, char *argv[]);
int initServer(uint16_t port);

const uint16_t INCOMMING_CONNECTION_QUEUE_MAX = 10;

int main(int argc, char *argv[]){
    uint16_t port;
    //parse input
    port = parseInput(argc, argv);
    
    int server_fd = initServer(port);
    if (server_fd == -1){
        printf("Error initializing server: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    

    printf("Listening on port %u\n",port);

    for (;;){
        struct sockaddr_in sockaddr_in_client;
        socklen_t sockaddr_in_client_length = sizeof(sockaddr_in_client);
        int client_fd = accept(server_fd,(struct sockaddr*)&sockaddr_in_client, &sockaddr_in_client_length);
        if (client_fd == -1){
            printf("Error accepting connection: %s",strerror(errno));
            continue;
        }
        char receive_buffer[RECEIVE_BUFFER_SIZE+1];
        memset(receive_buffer, '\0', RECEIVE_BUFFER_SIZE+1);

        int received_bytes = recv(client_fd, receive_buffer, RECEIVE_BUFFER_SIZE, 0);
        if (received_bytes == -1){
            printf("Error recv(): %s\n",strerror(errno));
            close(client_fd);
            continue;
        }

        //parse request
        if(strncmp(receive_buffer, "GET",3)==0){
            char *resource = NULL;
            strtok(receive_buffer," ");
            resource = strtok(NULL,"\n");
            printf("%s\n",resource);
        }

        char *dummy_404=
        "HTTP/1.1 404 Not Found\r\n"
        "\r\n"
        "<html><h4>Sorry bro</h4></html>";
        send(client_fd, dummy_404, strlen(dummy_404), 0);


        close(client_fd);
    }

}

uint16_t parseInput(int argc, char *argv[]){
    if (argc > 2){
        printf("USAGE: ./server [port(default=8000)]");
        exit(EXIT_FAILURE);
    }else if (argc == 2){
        return atoi(argv[1]);
    }else{
        return 8000;
    }
}

int initServer(uint16_t port){
    //create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)return -1;    //bind
    struct sockaddr_in sockaddr_in_server;
    sockaddr_in_server.sin_family = AF_INET;
    sockaddr_in_server.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr_in_server.sin_port = htons(port);
    socklen_t sockaddr_in_server_length = sizeof(sockaddr_in_server);
    
    int result = bind(server_fd, (struct sockaddr *)&sockaddr_in_server, sockaddr_in_server_length);
    if (result == -1){
        close(server_fd);
        return -1;
    }

    result = listen(server_fd, INCOMMING_CONNECTION_QUEUE_MAX);
    if (result == -1){
        close(server_fd);
        return -1;
    }
    return server_fd;
}