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
void GetFileSize(char *file_dir, long* bytes_read);
void *RequestFileExtensionParse(void* client_socket);
void *RequestFindFile(int cli_socket, char *method_http_request, char *file_path_http_request,char *file_extension_request);
double calculateTransferRate(unsigned long long bytes, unsigned long long microseconds);
void read_file_by_parts(const char* filename, unsigned int part_size_kb, unsigned int max_throughput_kb, int socket);

int main(int argc, char const* argv[]) {
    caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(HTTP_PORT);

    sem_init(&semaforo, 0, 1);
    max_vazao_server = atoi(argv[1]);
    printf("Vazao Maxima do Server: %d \n",max_vazao_server);

    int csize  = sizeof caddr;
    int server_connection = initServer(caddr,saddr);
    
    while (1) {
  
            int client_socket_connection;
            pthread_t thread_id;

            while((max_vazao_server - total_taxa_server) < 0) {
                printf("O servidor atingiu o maximo de vazão");
                printf("aguardando novas conexões\n");
                sleep(2);
            }

            // Aceitar uma conexão de cliente
            client_socket_connection = accept(server_connection, (struct sockaddr *)&caddr, &csize);
            // Criar uma nova thread para atender o cliente
            pthread_create(&thread_id, NULL, RequestFileExtensionParse, (void *)&client_socket_connection);
            printf("EXECUTEI");
            //pthread_exit(&thread_id);
         
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


    // Lê a requisição do cliente
    memset(buffer, 0, BUFFER_SIZE);
    if (read(socket_cli, buffer, BUFFER_SIZE - 1) < 0) {
        perror("Erro ao ler a requisição do cliente");
        exit(1);
    }

    method_http_request = strtok(buffer, " \t\n");
    file_path_http_request = strtok(NULL, " \t");
    if(strcmp(file_path_http_request,"/")!=0){
        file_extension_request = strtok(file_path_http_request,".");
        file_extension_request = strtok(NULL, " \t");
    }
    else {
        file_path_http_request = "/index";
        file_extension_request = ".";
        file_extension_request = "html";
    }

    printf("Client ID: %d VERBO %s PATH %s EXT %s \n",socket_cli,method_http_request,file_path_http_request,file_extension_request);
    RequestFindFile(socket_cli,method_http_request,file_path_http_request,file_extension_request);
}

void * RequestFindFile(int cli_socket, char *method_http_request, char *file_path_http_request,char *file_extension_request) {
    struct timeval  timeval1, timeval2;
    struct timespec req = {0};
    
    char file_dir[FILENAME_MAX],current_dir[FILENAME_MAX];
    char response[BUFFER_SIZE];
    long bytes_read;

    req.tv_sec = 0; 
    FILE *fp;
    
    // Obtendo o diretório dos arquivos estaticos html
    GetCurrentDir(current_dir, FILENAME_MAX);
    strcpy(file_dir,current_dir);
    strcat(file_dir,"/htdocs");
    strcat(file_dir,file_path_http_request);
    strcat(file_dir,".");
    strcat(file_dir,file_extension_request);

    fp = fopen(file_dir, "r");
    
    if (fp != NULL) {

        GetFileSize(file_dir,&bytes_read);
        
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(caddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        
        int taxa = checkIPExistence(clientIP);

	    if(strcmp(file_extension_request,"html") == 0 ){
        
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/%s\r\nConnection: keep-alive\r\nKeep-Alive: timeout=5, max=1000\r\nContent-Length: %ld\r\n\r\n", file_extension_request,bytes_read);
            write(cli_socket, response, strlen(response));
            
            gettimeofday(&timeval1, NULL);
            read_file_by_parts(file_dir,1024,taxa,cli_socket);
            gettimeofday(&timeval2, NULL);
            
            close(cli_socket);  
            double rtt_html = (timeval2.tv_sec - timeval1.tv_sec) * 1000000 + (timeval2.tv_usec - timeval1.tv_usec);
            printf("RTT de envio do HTML: %fms \n",rtt_html);      
        
        }else{
            printf("Endereço IP do cliente: %s\n", clientIP);
            // Aguarda permissão para acessar a variável "taxa"
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: image/%s\r\nConnection: keep-alive\r\nKeep-Alive: timeout=5, max=1000\r\nContent-Length: %ld\r\n\r\n", file_extension_request,bytes_read);
            write(cli_socket, response, strlen(response));
            read_file_by_parts(file_dir,1024,taxa,cli_socket);
            close(cli_socket);

        }
    }
    else {// se nao encontrar arquivo entra aqui{
        write(cli_socket, "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
        close(cli_socket);
    }
    fclose(fp);
}

void GetFileSize(char *file_dir, long* bytes_read){
    FILE *fp;
    char *buffer_file;

    fp = fopen(file_dir, "r");
          
    fseek(fp, 0, SEEK_END);
    *bytes_read = ftell(fp);

    fclose(fp);
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

void read_file_by_parts(const char* filename, unsigned int part_size_kb, unsigned int max_throughput_kb, int socket) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    // Calcula o tamanho máximo da parte com base na taxa de transferência
    unsigned int part_size_bytes = part_size_kb * 1024;
    unsigned int max_throughput_bytes = max_throughput_kb * 1024;
    unsigned int max_part_size = part_size_bytes;
    if (max_part_size > max_throughput_bytes) {
        max_part_size = max_throughput_bytes;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read = 0;
    size_t total_bytes_read = 0;

    struct timeval start_time, end_time;
    
    gettimeofday(&start_time, NULL);  // Inicia a contagem de tempo

    while ((bytes_read = fread(buffer, sizeof(char), 10, file)) > 0) {
        total_bytes_read += bytes_read;
        write(socket, buffer, bytes_read);
        // Verifica se atingiu o tamanho máximo da parte
        if (total_bytes_read >= max_part_size) {
            // Aguarda o tempo necessário para respeitar a taxa de transferência
            usleep((total_bytes_read * 1000000) / max_throughput_bytes);
            total_bytes_read = 0;  // Reinicia o contador
        }
    }

    gettimeofday(&end_time, NULL);  // Finaliza a contagem de tempo

    fclose(file);

    double start_seconds = start_time.tv_sec + (start_time.tv_usec / 1000000.0);
    double end_seconds = end_time.tv_sec + (end_time.tv_usec / 1000000.0);
    double elapsed_time = end_seconds - start_seconds;
    printf("Tempo decorrido: %.2f segundos\n", elapsed_time);
}