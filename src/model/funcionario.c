#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

int TFunc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char)*B_NOME 
         + sizeof(char)*B_CPF 
         + sizeof(char)*B_DATA_NASCIMENTO 
         + sizeof(double);
}

TFunc TFunc_New(const char* nome, const char* cpf, const char* data_nascimento, double salario) {
    TFunc func = {0};
    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= B_NOME || strlen(cpf) >= B_CPF || strlen(data_nascimento) >= B_DATA_NASCIMENTO) {
        perror("TFunc buffer overflow");
        return func;
    }
    // Copiar os dados para a estrutura TFunc usando strncpy_s
    // `sizeof(func.nome)` é o tamanho do buffer, e `_TRUNCATE` indica que a string deve ser truncada se for maior que o buffer
    strncpy_s(func.nome, sizeof(func.nome), nome, _TRUNCATE);
    strncpy_s(func.cpf, sizeof(func.cpf), cpf, _TRUNCATE);
    strncpy_s(func.data_nascimento, sizeof(func.data_nascimento), data_nascimento, _TRUNCATE);
    func.salario = salario;
    return func;
}

// Função para buscar um funcionário pelo ID na tabela
TFunc TFunc_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TFunc func = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return func;
    }
    int index = DB_FindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return func;
    }
    // binary search
    // Obter os offsets de início e fim da tabela
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    // Calcular o número de registros
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size; // -1
    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        size_t seek = start_offset + (middle * size);
        fseek(file, seek, SEEK_SET);
        func = TFunc_Read(file);
        if (func.pk == pk) {
            return func;
        }
        if (func.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return func;
}

TFunc TFunc_Read(FILE *file) {
    TFunc func = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return func;
    }
    if (fread(&func.pk, sizeof(unsigned long), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return func;
    }
    fread(func.nome, sizeof(char), sizeof(func.nome), file);
    fread(func.cpf, sizeof(char), sizeof(func.cpf), file);
    fread(func.data_nascimento, sizeof(char), sizeof(func.data_nascimento), file);
    fread(&func.salario, sizeof(double), 1, file);
    return func;
}

void TFunc_Print(TFunc *func) {
    if (func == NULL) {
        perror("Funcionário nulo\n");
        return;
    }
    printf("# Funcionário:\n");
    printf("| PK: %lu\n", func->pk);
    printf("| Nome: %s\n", func->nome);
    printf("| CPF: %s\n", func->cpf);
    printf("| Data de Nascimento: %s\n", func->data_nascimento);
    printf("| Salário: %.2lf\n", func->salario);
}   