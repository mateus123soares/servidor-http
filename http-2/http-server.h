 //DECLARACAO DA FUNCOES
int initServer(struct sockaddr_in caddr,struct sockaddr_in saddr);
void *findFile(int cli_socket, char *method_http_request, char *file_path_request, char *file_extension_request);
char* fileLength(char *file_dir, long* bytes_read);
void *handleRequestFilePath(void* pcli_socket);