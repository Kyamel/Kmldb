#include <stdio.h>
#include <time.h>

// Função para iniciar o log
FILE* init_log() {
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("Erro ao abrir o arquivo de log");
        return NULL;
    }
    return log_file;
}

// Função para registrar uma tentativa no log
void log_attempt(FILE* log_file, long unsigned middle, long unsigned pk) {
    if (log_file) {
        fprintf(log_file, "Tentativa: posicao=%lu, pk=%lu\n", middle, pk);
    }
}

// Função para calcular e registrar o tempo total
void log_total_time(FILE* log_file, clock_t start_time) {
    if (log_file) {
        clock_t end_time = clock();
        double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        fprintf(log_file, "Tempo total de busca binaria: %.2f segundos\n", total_time);
    }
}