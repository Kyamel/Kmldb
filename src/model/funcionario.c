#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

int TFunc_Size() {
    return sizeof(int) 
         + sizeof(char)*B_NOME 
         + sizeof(char)*B_CPF 
         + sizeof(char)*B_DATA_NASCIMENTO 
         + sizeof(double);
}

TFunc TFunc_New(FILE *file, const char* table_name, const char* nome, const char* cpf, const char* data_nascimento, double salario) {
    TFunc func = {0};
    if(strlen(nome) >= B_NOME || strlen(cpf) >= B_CPF || strlen(data_nascimento) >= B_DATA_NASCIMENTO) {
        perror("TFunc buffer overflow");
        return func;
    }
    // Carregar o cabeçalho do banco de dados
    DatabaseHeader* header = DB_LoadHeader(file);
    if (header == NULL) {
        perror("Erro ao carregar o cabeçalho do banco de dados");
        return func;
    }
    // Encontrar a tabela pelo nome
    int index = DB_FindTable(file, table_name);
    // Verificar se a tabela foi encontrada
    if (index == -1) {
        perror("Tabela não encontrada");
        return func;
    }
    // Atribuir o próximo ID disponível da tabela
    func.id = header->tables[index].next_id++;
    // Copiar os dados para a estrutura TFunc
    strncpy(func.nome, nome, B_NOME - 1);  // Garantir que a string seja terminada com \0
    func.nome[B_NOME - 1] = '\0';          // Garantir terminação nula
    strncpy(func.cpf, cpf, B_CPF - 1);
    func.cpf[B_CPF - 1] = '\0';
    strncpy(func.data_nascimento, data_nascimento, B_DATA_NASCIMENTO - 1);
    func.data_nascimento[B_DATA_NASCIMENTO - 1] = '\0';
    func.salario = salario;
    // Reposiciona para escrever o cabeçalho de volta no arquivo
    fseek(file, 0, SEEK_SET);
    fwrite(header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
    // Libera memória
    free(header);
    return func;
}

// Função para buscar um funcionário pelo ID na tabela
TFunc TFunc_GetByPK(FILE *file, const char* table_name, int id) {
    TFunc func = {0}; // Inicializa com valores padrão
    DatabaseHeader *header = DB_LoadHeader(file);
    if (header == NULL) {
        perror("Erro ao carregar o cabeçalho do banco de dados");
        exit(EXIT_FAILURE);
    }
    int index = DB_FindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        free(header);
        return func;
    }
    // Posicionar no início da tabela
    fseek(file, header->tables[index].start_offset, SEEK_SET);
    TFunc *f = NULL;
    while (ftell(file) < header->tables[index].end_offset) {
        if (f != NULL) {
            free(f);
        }
        f = TFunc_Read(file);
        if (f->id == id) {
            func = *f;
            free(f);
            free(header);
            return func;
        }
    }
    free(f);  // Libera o último alocado, se houver
    free(header);
    return func;
}

TFunc *TFunc_Read(FILE *(file)){
    TFunc *func = (TFunc *) malloc(sizeof(TFunc));

    if (0 >= fread(&func->id, sizeof(int), 1, file)){
        free(func);
        return NULL;
    }

    fread(func->nome, sizeof(char), sizeof(func->nome), file);
    fread(func->cpf, sizeof(char), sizeof(func->cpf), file);
    fread(func->data_nascimento, sizeof(char), sizeof(func->data_nascimento), file);
    fread(&func->salario, sizeof(double), 1, file);
    return func;
}

void TFunc_Print(TFunc func) {
    printf("# Funcionário:\n");
    printf("| ID: %d\n", func.id);
    printf("| Nome: %s\n", func.nome);
    printf("| CPF: %s\n", func.cpf);
    printf("| Data de Nascimento: %s\n", func.data_nascimento);
    printf("| Salário: %.2lf\n", func.salario);
}   