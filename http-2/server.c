// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define GetCurrentDir getcwd

#include "http-server.h"

int checkIPExistence(const char* ip);
void saveIP(const char* ip);
char* GetFileSize(char *file_dir, long* bytes_read);
void *RequestFileExtensionParse(void* client_socket);
void *RequestFindFile(int cli_socket, char *method_http_request, char *file_path_http_request,char *file_extension_request);
double calculateTransferRate(unsigned long long bytes, unsigned long long microseconds);

int main(int argc, char const* argv[]) {
    caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(HTTP_PORT);

    sem_init(&semaforo, 0, 1);
    int csize  = sizeof caddr;
    int max_vazao_server = atoi(argv[1]);
    printf("Vazao Maxima do Server: %d \n",max_vazao_server);
    int server_connection = initServer(caddr,saddr);
    
    while (1) {
        if (total_taxa_server < max_vazao_server){
            int client_socket_connection;
            pthread_t thread_id;

            // Aceitar uma conexão de cliente
            client_socket_connection = accept(server_connection, (struct sockaddr *)&caddr, &csize);
            printf("Cliente conectado: %d",client_socket_connection);
            // Criar uma nova thread para atender o cliente
            pthread_create(&thread_id, NULL, RequestFileExtensionParse, (void *)&client_socket_connection);
        }
        else {
             printf("O servidor atingiu o maximo de vazão");
             break;
        }
    }
    
    // Fechar o socket do servidor
    close(server_connection);
	return 0;
}

void *RequestFileExtensionParse(void* client_socket){
    // Iniciando variaveis
    int socket_cli = *((int*)client_socket);
    char *method_http_request,*file_path_http_request, *file_extension_request;
	char buffer[BUFFER_SIZE] = { 0 };
	read(socket_cli, buffer, BUFFER_SIZE);
    // Realizando o parse da requisição realizada do usuário: METODO ARQUIVO EXTENÇÃO 
    method_http_request = strtok(buffer, " \t\n");

    file_path_http_request = strtok(NULL, " \t");
    file_extension_request = strtok(file_path_http_request,".");
    file_extension_request = strtok(NULL, " \t");

    printf("%d %s %s %s \n",socket_cli,method_http_request,file_path_http_request,file_extension_request);
    RequestFindFile(socket_cli,method_http_request,file_path_http_request,file_extension_request);
}

void * RequestFindFile(int cli_socket, char *method_http_request, char *file_path_http_request,char *file_extension_request) {
    struct timeval  timeval1, timeval2;
    struct timespec req = {0};
    req.tv_sec = 0; 
    
    char file_dir[FILENAME_MAX],current_dir[FILENAME_MAX];
    char *buffer;
    long bytes_read;
    char response[BUFFER_SIZE];

    FILE *fp;
    
    // Obtendo o diretório dos arquivos estaticos html
    GetCurrentDir(current_dir, FILENAME_MAX);
    strcpy(file_dir,current_dir);
    strcat(file_dir,"/htdocs");

    if(strcmp(file_path_http_request,"/")==0){
        strcat(file_dir,"/index.html");
    }
    else{
        strcat(file_dir,file_path_http_request);
        strcat(file_dir,".");
        strcat(file_dir,file_extension_request);
    }

    fp = fopen(file_dir, "r");
    if (fp != NULL) {
        buffer=GetFileSize(file_dir,&bytes_read);
        int TAXA_MAX=0;
        size_t bytesRead;
        char clientIP[INET_ADDRSTRLEN];
        int totalBytesWritten;
        inet_ntop(AF_INET, &(caddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        
	    if(strcmp(file_extension_request,"html") == 0 ){
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/%s\r\nContent-Length: %ld\r\n\r\n", file_extension_request,bytes_read);
            write(cli_socket, response, strlen(response));
            gettimeofday(&timeval1, NULL);
            write(cli_socket, buffer, bytes_read);
            gettimeofday(&timeval2, NULL);
            close(cli_socket);  
            double rtt_html = (double) (timeval2.tv_usec - timeval1.tv_usec) / 1000000 + (double) (timeval2.tv_sec - timeval1.tv_sec);
            printf("RTT de envio do HTML: %fs \n",rtt_html);      
        }else{
            printf("Endereço IP do cliente: %s\n", clientIP);
            // Aguarda permissão para acessar a variável "taxa"
            sem_wait(&semaforo);
            TAXA_MAX=checkIPExistence(clientIP);
            total_taxa_server = total_taxa_server + 1000;
            // Libera o semáforo para permitir que outra thread acesse a variável
            sem_post(&semaforo);
            printf("Taxa: %d \n",total_taxa_server);
            char max_taxa[TAXA_MAX];
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: image/%s\r\nContent-Length: %ld\r\n\r\n", file_extension_request,bytes_read);
            write(cli_socket, response, strlen(response));
            gettimeofday(&timeval1, NULL);
                while ((bytesRead = fread(max_taxa, 1, sizeof(max_taxa), fp)) > 0) {
                    write(cli_socket, max_taxa, bytesRead);
                    totalBytesWritten += bytesRead;
                }
            gettimeofday(&timeval2, NULL);

            sem_wait(&semaforo);
            // Aguarda permissão para acessar a variável "taxa"
            total_taxa_server = TAXA_MAX - total_taxa_server;
            // Libera o semáforo para permitir que outra thread acesse a variável
            sem_post(&semaforo);
            printf("Taxa: %d \n",total_taxa_server);

            close(cli_socket);  
            double rtt_img = (timeval2.tv_sec - timeval1.tv_sec) * 1000000 + (timeval2.tv_usec - timeval1.tv_usec);
            printf("Total Bytes send: %d \n",totalBytesWritten);
            double largura_de_banda =  calculateTransferRate(totalBytesWritten,rtt_img);
            

            printf("Atraso fim-a-fim: %.2f ms\n", rtt_img);
            printf("Largura da banda: %.2f kbps \n",largura_de_banda); 
            printf("Taxa Utilizada: %ld\n", sizeof(max_taxa));
        }
    }
    else {// se nao encontrar arquivo entra aqui{
        write(cli_socket, "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
        close(cli_socket);
    }
}

char* GetFileSize(char *file_dir, long* bytes_read){
    FILE *fp;
    char *buffer_file;

    fp = fopen(file_dir, "r");
          
    fseek(fp, 0, SEEK_END);
    long bytes_read_num = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    buffer_file = (char *)malloc(bytes_read_num * sizeof(char));
    *bytes_read=bytes_read_num;
    
    fread(buffer_file, bytes_read_num, 1, fp); // lê o buffer
    fclose(fp);
    return buffer_file;
}

int checkIPExistence(const char* ip) {
    int taxa;
    char ipFromFile[16];
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        return false;
    }

    char line[MAX_IP_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%[^,],%d", ipFromFile, &taxa);
        if (strcmp(ipFromFile, ip) == 0) {
            return taxa;
        }
    }

    fclose(file);
    return 10000;
}

void saveIP(const char* ip) {
    FILE* file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fprintf(file, "%s\n", ip);
    fclose(file);
}

double calculateTransferRate(unsigned long long bytes, unsigned long long microseconds) {
    double rate = (bytes * 8) / (microseconds / 1000000.0); // Calcula a taxa de transferência em kbps
    return rate;
}