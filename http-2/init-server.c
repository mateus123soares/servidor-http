#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CONNECTIONS 5
#define HTTP_PORT 50001

int initServer(struct sockaddr_in caddr, struct sockaddr_in saddr){
    
    int server=socket(AF_INET, SOCK_STREAM, 0);

    if (server == -1){
        perror("socket failed\n");
        exit(1);
    }
    else {
        printf("Socket criado\n");
    };

    if (bind(server, (struct sockaddr *) &saddr, sizeof saddr) == -1){
        perror("bind failed\n");
        exit(1);
    }
    else {
        printf("Realizado o Bind na Porta: %d \n",HTTP_PORT);
    };

    if(listen(server, MAX_CONNECTIONS) == -1){
        perror("listen failed\n");
        exit(1);
    }
    else {
        printf("Server listen\n");
    };

    return server;
};