#include "treino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

// Calcula o tamanho da estrutura TTreino
int TTreino_Size() {
    return sizeof(long unsigned) // pk
         + sizeof(long unsigned) // cpk
         + sizeof(long unsigned) // epk
         + sizeof(char) * BT_NOME // nome
         + sizeof(char) * BT_TIPO; // tipo
}

// Cria uma nova instância de TTreino
// Cria uma nova instância de TTreino
TTreino TTreino_New(const char* nome, const char* tipo, long unsigned cpk, long unsigned epk) {
    TTreino treino = {0};
    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BT_NOME || strlen(tipo) >= BT_TIPO) {
        perror("TTreino buffer overflow");
        return treino;
    }

    strncpy_s(treino.nome, sizeof(treino.nome), nome, _TRUNCATE);
    strncpy_s(treino.tipo, sizeof(treino.tipo), tipo, _TRUNCATE);
    treino.cpk = cpk;
    treino.epk = epk;

    return treino;
}
// Função para buscar um treino pelo ID na tabela
TTreino TTreino_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TTreino treino = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return treino;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return treino;
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
        treino = TTreino_Read(file);
        if (treino.pk == pk) {
            return treino;
        }
        if (treino.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return treino;
}

// Função para ler um treino de um arquivo
TTreino TTreino_Read(FILE *file) {
    TTreino treino = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return treino;
    }
    if (fread(&treino.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return treino;
    }
    if (fread(&treino.cpk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do Cliente PK");
        return treino;
    }
    if (fread(&treino.epk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do Exercicio PK");
        return treino;
    }
    fread(treino.nome, sizeof(char), sizeof(treino.nome), file);
    fread(treino.tipo, sizeof(char), sizeof(treino.tipo), file);
    return treino;
}

// Função para imprimir os dados de um treino
void TTreino_Print(TTreino *treino) {
    if (treino == NULL) {
        perror("TTreino nulo\n");
        return;
    }
    printf("# TTreino:\n");
    printf("| PK: %lu\n", treino->pk);
    printf("| Cliente PK: %lu\n", treino->cpk);
    printf("| Exercício PK: %lu\n", treino->epk);
    printf("| Nome: %s\n", treino->nome);
    printf("| Tipo: %s\n", treino->tipo);
}
