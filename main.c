#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024

void read_file_by_parts(const char* filename, unsigned int part_size_kb, unsigned int max_throughput_kb) {
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

    while ((bytes_read = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, file)) > 0) {
        total_bytes_read += bytes_read;

        // Verifica se atingiu o tamanho máximo da parte
        if (total_bytes_read >= max_part_size) {
            // Aguarda o tempo necessário para respeitar a taxa de transferência
            usleep((total_bytes_read * 1000000) / max_throughput_bytes);
            total_bytes_read = 0;  // Reinicia o contador
        }

        // Processa os dados lidos
        // ...

        // Exemplo: escreve os dados lidos no console
        //fwrite(buffer, sizeof(unsigned char), bytes_read, stdout);
        printf("Bytes %ld\n", total_bytes_read);

    }

    gettimeofday(&end_time, NULL);  // Finaliza a contagem de tempo

    fclose(file);

    double start_seconds = start_time.tv_sec + (start_time.tv_usec / 1000000.0);
    double end_seconds = end_time.tv_sec + (end_time.tv_usec / 1000000.0);
    double elapsed_time = end_seconds - start_seconds;
    printf("Tempo decorrido: %.2f segundos\n", elapsed_time);
}

int main() {
    const char* filename = "unipampa.png";
    unsigned int part_size_kb, max_throughput_kb;

    printf("Tamanho da parte (em KB): ");
    scanf("%u", &part_size_kb);

    printf("Taxa de transferência máxima (em KB/s): ");
    scanf("%u", &max_throughput_kb);

    read_file_by_parts(filename, part_size_kb, max_throughput_kb);

    return 0;
}
