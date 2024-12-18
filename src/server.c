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

#define RECEIVE_BUFFER_SIZE 1024

typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_UNKNOWN
}HttpMethod;

typedef enum {
    HTTP_VERSION_09,
    HTTP_VERSION_UNKNOWN
}HttpVersion;

typedef struct {
    char *resource;
    uint16_t resource_len;
    HttpMethod method;
    HttpVersion version;
}HttpRequest;

typedef struct {

}HttpResponse;

void sigintHandler(int signo);
void terminate(int exit_code);
void parseInput(int argc, char *argv[]);
int serverInit(uint16_t port);
int httpParseRequest(char *request_raw, HttpRequest *request);
int httpProccessRequest(HttpRequest request, HttpResponse *response);
int httpSendResponse(HttpResponse response);

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

    for(;;){
        int client_fd;
        struct sockaddr_in client_sockaddr_in;
        socklen_t client_sockaddr_in_len = sizeof(client_sockaddr_in);
        if( -1 == (client_fd = accept(server_fd, (struct sockaddr*)&client_sockaddr_in, &client_sockaddr_in_len)) ){
            printf("Error trying to initiate new connection: %s\n",strerror(errno));
            continue;
        }
        //handle connection
        char recv_buffer[RECEIVE_BUFFER_SIZE+1];
        memset(recv_buffer, '\0', RECEIVE_BUFFER_SIZE+1);
        int received_bytes;
        received_bytes = read(client_fd, recv_buffer, RECEIVE_BUFFER_SIZE);
        if ( received_bytes == -1 ){
            printf("Failed to receive bytes from incomming connection: %s\n",strerror(errno));
            close(client_fd);
            continue;
        }
        HttpRequest request;
        if( -1 == httpParseRequest(recv_buffer, &request) ){
            printf("Error parsing request\n");
            close(client_fd);
            continue;
        }
        HttpResponse response;
        if ( -1 == httpProccessRequest(request, &response) ){
            printf("Error proccessing request.\n");
            close(client_fd);
            continue;
        }

        if ( -1 == httpSendResponse(response) ){
            printf("Error responding to client,\n");
            close(client_fd);
            continue;
        }

        close(client_fd);
    }

    terminate(EXIT_SUCCESS);
}

int httpSendResponse(HttpResponse response){
    return 0;
}

int httpProccessRequest(HttpRequest request, HttpResponse *response){
    return 0;
}

int httpParseRequest(char *request_raw, HttpRequest *request){
    char *line1 = strtok( request_raw, "\r\n" );
    request_raw = strtok(NULL,"");
    char *method = strtok(line1," ");
    char *resource = strtok(NULL," ");
    char *version = strtok(NULL," ");
    
    //get http method
    if ( strncmp("GET",method,3) == 0 ){
        request->method = HTTP_METHOD_GET;
    }else{
        request->method = HTTP_METHOD_UNKNOWN;
        return -1;
    }

    //get resource
    request->resource_len = strlen(resource);
    request->resource = malloc((request->resource_len+1)*sizeof(char));
    if( request->resource == NULL )
    {
        printf("Error allocating memory: %s\n",strerror(errno));
        return -1;
    }
    request->resource[request->resource_len] = 0;
    request->resource = strncpy(request->resource, resource, request->resource_len);

    //get version
    if ( version == NULL ){
        request->version = HTTP_VERSION_09;
    }else{
        request->version = HTTP_VERSION_UNKNOWN;
    }
    return 0;
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