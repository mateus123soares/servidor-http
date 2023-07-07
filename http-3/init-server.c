#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

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

    // Habilita o keep-alive no socket
    int enableKeepAlive = 1;
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive)) < 0) {
        perror("Erro ao habilitar o keep-alive");
        exit(EXIT_FAILURE);
    }

    // Configura os parâmetros de keep-alive
    int keepIdle = 60; // Tempo de ociosidade em segundos
    int keepInterval = 5; // Intervalo de sondagem em segundos
    int keepCount = 3; // Número de falhas antes da desconexão

    if (setsockopt(server, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle)) < 0 ||
        setsockopt(server, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval)) < 0 ||
        setsockopt(server, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount)) < 0) {
        perror("Erro ao configurar os parâmetros de keep-alive");
        exit(EXIT_FAILURE);
    }

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