// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#define GetCurrentDir getcwd

#define PORT 8080
#define MAX_CONNECTIONS 5
#define MESSAGE_SIZE 2048
#define HTTP_PORT 50000
#define MAX_FILE_BUFF 2136038
#define SERVER_NAME "server-redes"

char* fileLength(char *file_dir, long* bytes_read);
void requestFileParser(int socket_cli);
void * findFile(int cli_socket, char *method_http_request, char *file_path_request,char *file_extension_request);

int main(int argc, char const* argv[]){
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char* hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,	sizeof(address))< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))	< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	//valread = read(new_socket, buffer, 1024);

	//printf("%s\n", buffer);
	requestFileParser(new_socket);
    //send(new_socket, hello, strlen(hello), 0);
	
    //printf("Hello message sent\n");

	// closing the connected socket
	close(new_socket);

	// closing the listening socket
	//shutdown(server_fd, SHUT_RDWR);

	return 0;
}

void requestFileParser(int socket_cli){

    int client_rsv;
    char message_rsv[MESSAGE_SIZE];
    char *method_http_request,*file_path_request, *file_extension_request;
	char buffer[1024] = { 0 };
	read(socket_cli, buffer, 1024);

    method_http_request = strtok(buffer, " \t\n");
    file_path_request = strtok(NULL, " \t");
    file_extension_request = strtok(file_path_request,".");
    file_extension_request = strtok(NULL, " \t");

	findFile(socket_cli,method_http_request,file_path_request,file_extension_request);
}

void * findFile(int cli_socket, char *method_http_request, char *file_path_request,char *file_extension_request) {
    struct timeval  timeval1, timeval2;
    struct timespec req = {0};
    req.tv_sec = 0; 
    
    char file_dir[FILENAME_MAX],current_dir[FILENAME_MAX];
    char header_buff [250];
    char *buffer;

    long bytes_read;

    FILE *fp;
    
    GetCurrentDir(current_dir, FILENAME_MAX);
    strcpy(file_dir,current_dir);
    strcat(file_dir,"/htdocs");

    if(strcmp(file_path_request,"/")==0){
        strcat(file_dir,"/index.html");
    }
    else{
        strcat(file_dir,file_path_request);
        strcat(file_dir,".");
        strcat(file_dir,file_extension_request);
    }

	if(strcmp(file_extension_request,"html") == 0 ){
        fp = fopen(file_dir, "r");
        if (fp == NULL) {

        }
        else{
            strcpy (header_buff, "HTTP/1.1 200 OK\r\n");
            strcat (header_buff, "Content-type: text/html\r\n");
            // strcat (header_buff, "Location: ");
            // strcat (header_buff, inet_ntoa(saddr.sin_addr));
            // strcat (header_buff, "\r\n");
            strcat (header_buff, "Server: ");
            strcat (header_buff, SERVER_NAME);
            strcat (header_buff, "\r\n");
            strcat (header_buff, "Connection: keep-alive\r\n\r\n");
            
            send(cli_socket, header_buff , strlen(header_buff)+1, 0);
    
            buffer=fileLength(file_dir,&bytes_read);

            gettimeofday(&timeval1, NULL);
            write (cli_socket, buffer, bytes_read); //envia html para cliente
            gettimeofday(&timeval2, NULL);
            
            double rtt_html = (double) (timeval2.tv_usec - timeval1.tv_usec) / 1000000 + (double) (timeval2.tv_sec - timeval1.tv_sec);
            printf("Enviando Request para CLIENT RTT: %fs \n",rtt_html);
            

        }
	}

}

char* fileLength(char *file_dir, long* bytes_read){
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