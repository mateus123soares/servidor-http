#define MAX_CONNECTIONS 5
#define MESSAGE_SIZE 2048
#define HTTP_PORT 50002
#define MAX_FILE_BUFF 2136038
#define SERVER_NAME "server-redes"
#define MAX_CLIENTES 100
#define BUFFER_SIZE 1024
#define MAX_IP_LENGTH 16
#define FILENAME "ips.txt"
#include <semaphore.h>

struct sockaddr_in caddr, saddr;
int total_taxa_server;
sem_t semaforo;

int initServer(struct sockaddr_in caddr,struct sockaddr_in saddr);