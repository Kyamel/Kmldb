#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

// Calcula o tamanho da estrutura TFunc
int TFunc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char) * BF_NOME 
         + sizeof(char) * BF_CPF 
         + sizeof(char) * BF_EMAIL 
         + sizeof(char) * BF_TELEFONE 
         + sizeof(char) * BF_DATA_NASCIMENTO 
         + sizeof(double);
}

// Cria uma nova instância de TFunc
TFunc TFunc_New(unsigned long pk, const char* nome, const char* cpf, const char* email, const char* telefone, const char* data_nascimento, double salario) {
    TFunc func = {0};

    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BF_NOME || strlen(cpf) >= BF_CPF || strlen(email) >= BF_EMAIL || strlen(telefone) >= BF_TELEFONE || strlen(data_nascimento) >= BF_DATA_NASCIMENTO) {
        perror("TFunc buffer overflow");
        return func;
    }

    // Dependendo do ambiente, usa strncpy_s ou strncpy
    #ifdef _MSC_VER
        // Ambiente Microsoft
        func.pk = pk;
        strncpy_s(func.nome, sizeof(func.nome), nome, _TRUNCATE);
        strncpy_s(func.cpf, sizeof(func.cpf), cpf, _TRUNCATE);
        strncpy_s(func.email, sizeof(func.email), email, _TRUNCATE);
        strncpy_s(func.telefone, sizeof(func.telefone), telefone, _TRUNCATE);
        strncpy_s(func.data_nascimento, sizeof(func.data_nascimento), data_nascimento, _TRUNCATE);
    #else
        // Ambiente Unix/Linux
        func.pk = pk;
        strncpy(func.nome, nome, BF_NOME - 1);
        func.nome[BF_NOME - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.cpf, cpf, BF_CPF - 1);
        func.cpf[BF_CPF - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.email, email, BF_EMAIL - 1);
        func.email[BF_EMAIL - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.telefone, telefone, BF_TELEFONE - 1);
        func.telefone[BF_TELEFONE - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.data_nascimento, data_nascimento, BF_DATA_NASCIMENTO - 1);
        func.data_nascimento[BF_DATA_NASCIMENTO - 1] = '\0'; // Garantir que a string está terminada
    #endif

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
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return func;
    }

    // Busca binária
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size;
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

// Função para ler um funcionário de um arquivo
TFunc TFunc_Read(FILE *file) {
    TFunc func = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return func;
    }
    if (fread(&func.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return func;
    }
    fread(func.nome, sizeof(char), sizeof(func.nome), file);
    fread(func.cpf, sizeof(char), sizeof(func.cpf), file);
    fread(func.email, sizeof(char), sizeof(func.email), file);
    fread(func.telefone, sizeof(char), sizeof(func.telefone), file);
    fread(func.data_nascimento, sizeof(char), sizeof(func.data_nascimento), file);
    fread(&func.salario, sizeof(double), 1, file);
    return func;
}

// Função para imprimir os dados de um funcionário
void TFunc_Print(TFunc *func) {
    if (func == NULL) {
        perror("Funcionário nulo\n");
        return;
    }
    printf("# Funcionario:\n");
    printf("| PK: %lu\n", func->pk);
    printf("| Nome: %s\n", func->nome);
    printf("| CPF: %s\n", func->cpf);
    printf("| Email: %s\n", func->email);
    printf("| Telefone: %s\n", func->telefone);
    printf("| Data de Nascimento: %s\n", func->data_nascimento);
    printf("| Salario: %.2lf\n", func->salario);
}
