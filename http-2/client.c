// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>

char* fileLength(char *file_dir, long* bytes_read);
void * findFile(int cli_socket, char *method_http_request, char *file_path_request,char *file_extension_request);
void *handleRequestFilePath(void* pcli_socket);

int main(int argc, char const* argv[]){
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)))< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	send(client_fd, hello, strlen(hello), 0);
	
    printf("Hello message sent\n");
	
    valread = read(client_fd, buffer, 1024);
	
    printf("%s\n", buffer);

	// closing the connected socket
	close(client_fd);

	return 0;
}