#define MAX_CONNECTIONS 5
#define MESSAGE_SIZE 2048
#define HTTP_PORT 50001
#define MAX_FILE_BUFF 2136038
#define SERVER_NAME "server-redes"
#define MAX_CLIENTES 100
#define BUFFER_SIZE 1024
#define MAX_IP_LENGTH 16
#define FILENAME "ips.txt"

struct sockaddr_in caddr, saddr;

int initServer(struct sockaddr_in caddr,struct sockaddr_in saddr);