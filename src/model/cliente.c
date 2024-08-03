#include "cliente.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

// Calcula o tamanho da estrutura TCliente
int TCliente_Size() {
    return sizeof(long unsigned) 
         + sizeof(char) * BC_NOME 
         + sizeof(char) * BC_CPF 
         + sizeof(char) * BC_EMAIL 
         + sizeof(char) * BC_TELEFONE 
         + sizeof(char) * BC_EXP_DATE;
}

// Cria uma nova instância de TCliente
TCliente TCliente_New(const char* nome, const char* cpf, const char* email, const char* telefone, const char* exp_date) {
    TCliente cliente = {0};

    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BC_NOME || strlen(cpf) >= BC_CPF || strlen(email) >= BC_EMAIL || strlen(telefone) >= BC_TELEFONE || strlen(exp_date) >= BC_EXP_DATE) {
        perror("TCliente buffer overflow");
        return cliente;
    }

    // Dependendo do ambiente, usa strncpy_s ou strncpy
    #ifdef _MSC_VER
        // Ambiente Microsoft
        strncpy_s(cliente.nome, sizeof(cliente.nome), nome, _TRUNCATE);
        strncpy_s(cliente.cpf, sizeof(cliente.cpf), cpf, _TRUNCATE);
        strncpy_s(cliente.email, sizeof(cliente.email), email, _TRUNCATE);
        strncpy_s(cliente.telefone, sizeof(cliente.telefone), telefone, _TRUNCATE);
        strncpy_s(cliente.exp_date, sizeof(cliente.exp_date), exp_date, _TRUNCATE);
    #else
        // Ambiente Unix/Linux
        strncpy(cliente.nome, nome, BC_NOME - 1);
        cliente.nome[BC_NOME - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.cpf, cpf, BC_CPF - 1);
        cliente.cpf[BC_CPF - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.email, email, BC_EMAIL - 1);
        cliente.email[BC_EMAIL - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.telefone, telefone, BC_TELEFONE - 1);
        cliente.telefone[BC_TELEFONE - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.exp_date, exp_date, BC_EXP_DATE - 1);
        cliente.exp_date[BC_EXP_DATE - 1] = '\0'; // Garantir que a string está terminada
    #endif

    return cliente;
}

// Função para buscar um cliente pelo ID na tabela
TCliente TCliente_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TCliente cliente = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return cliente;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return cliente;
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
        cliente = TCliente_Read(file);
        if (cliente.pk == pk) {
            return cliente;
        }
        if (cliente.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return cliente;
}

// Função para ler um cliente de um arquivo
TCliente TCliente_Read(FILE *file) {
    TCliente cliente = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return cliente;
    }
    if (fread(&cliente.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return cliente;
    }
    fread(cliente.nome, sizeof(char), sizeof(cliente.nome), file);
    fread(cliente.cpf, sizeof(char), sizeof(cliente.cpf), file);
    fread(cliente.email, sizeof(char), sizeof(cliente.email), file);
    fread(cliente.telefone, sizeof(char), sizeof(cliente.telefone), file);
    fread(cliente.exp_date, sizeof(char), sizeof(cliente.exp_date), file);
    return cliente;
}

// Função para imprimir os dados de um cliente
void TCliente_Print(TCliente *cliente) {
    if (cliente == NULL) {
        perror("Cliente nulo\n");
        return;
    }
    printf("# Cliente:\n");
    printf("| PK: %lu\n", cliente->pk);
    printf("| Nome: %s\n", cliente->nome);
    printf("| CPF: %s\n", cliente->cpf);
    printf("| Email: %s\n", cliente->email);
    printf("| Telefone: %s\n", cliente->telefone);
    printf("| Data de Expiração: %s\n", cliente->exp_date);
}
