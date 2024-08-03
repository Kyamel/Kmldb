#include "exercicio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

// Calcula o tamanho da estrutura TExerc
int TExerc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char)*BE_NOME 
         + sizeof(char)*BE_TIPO 
         + sizeof(int);
}

// Cria uma nova instância de TExerc
TExerc TExerc_New(const char* nome, const char* tipo, int duration) {
    TExerc exec = {0};
    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BE_NOME || strlen(tipo) >= BE_TIPO) {
        perror("TExerc buffer overflow");
        return exec;
    }
    // Copiar os dados para a estrutura TExerc usando strncpy_s
    strncpy_s(exec.nome, sizeof(exec.nome), nome, _TRUNCATE);
    strncpy_s(exec.tipo, sizeof(exec.tipo), tipo, _TRUNCATE);
    exec.duration = duration;
    return exec;
}

// Função para buscar uma execução pelo ID na tabela
TExerc TExerc_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TExerc exec = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return exec;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return exec;
    }
    // binary search
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size;
    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        size_t seek = start_offset + (middle * size);
        fseek(file, seek, SEEK_SET);
        exec = TExerc_Read(file);
        if (exec.pk == pk) {
            return exec;
        }
        if (exec.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return exec;
}

// Função para ler uma execução de um arquivo
TExerc TExerc_Read(FILE *file) {
    TExerc exec = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return exec;
    }
    if (fread(&exec.pk, sizeof(unsigned long), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return exec;
    }
    fread(exec.nome, sizeof(char), sizeof(exec.nome), file);
    fread(exec.tipo, sizeof(char), sizeof(exec.tipo), file);
    fread(&exec.duration, sizeof(int), 1, file);
    return exec;
}

// Função para imprimir os dados de uma execução
void TExerc_Print(TExerc *exec) {
    if (exec == NULL) {
        perror("Exercício nula\n");
        return;
    }
    printf("# Exercicio:\n");
    printf("| PK: %lu\n", exec->pk);
    printf("| Nome: %s\n", exec->nome);
    printf("| Tipo: %s\n", exec->tipo);
    printf("| Duracao: %d segundos\n", exec->duration);
}
