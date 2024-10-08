#include <stdio.h>
#include <time.h>

// Função para iniciar o log
FILE* init_log() {
    FILE *log_file = NULL;
    errno_t err = fopen_s(&log_file, "dblog.txt", "a");
    if (err != 0) {
        perror("Erro ao abrir o arquivo de log");
        return NULL;
    }

    fprintf_s(log_file, "\nLog inicializado.\n");
    return log_file;
}

void close_log(FILE* log_file) {
    if (log_file) {
        fclose(log_file);
    }
}

// Função para registrar uma tentativa no log
void log_attempt(FILE* log_file, long unsigned middle, long unsigned pk) {
    if (log_file) {
        fprintf_s(log_file, "Tentativa: posicao=%lu, pk=%lu\n", middle, pk);
    }
}

// Função para calcular e registrar o tempo total
void log_total_time(FILE* log_file, clock_t start_time) {
    if (log_file) {
        clock_t end_time = clock();
        double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        fprintf_s(log_file, "Tempo total de busca binaria: %f segundos\n", total_time);
    }
}