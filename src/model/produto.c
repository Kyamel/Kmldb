#include "produto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

int TProd_Size() {
    return sizeof(long unsigned)
          + sizeof(char) * B_NOME
          + sizeof(char) * B_DESCRICAO
          + sizeof(char) * B_CATEGORIA
          + sizeof(char) * B_CODIGO_BARRAS
          + sizeof(double)
          + sizeof(int);
}

TProd TProd_New(const char* nome, const char* descricao, const char* categoria, const char* codigo_barras, double preco, int quantidade) {
    TProd prod = {0};
    // Verifica se as strings são muito grandes
    if (strlen(nome) >= B_NOME || strlen(descricao) >= B_DESCRICAO || strlen(categoria) >= B_CATEGORIA || strlen(codigo_barras) >= B_CODIGO_BARRAS) {
        perror("TProd buffer overflow");
        return prod;
    }
    // Copia os dados para a estrutura TProd usando strncpy_s com _TRUNCATE
    strncpy_s(prod.nome, sizeof(prod.nome), nome, _TRUNCATE);
    strncpy_s(prod.descricao, sizeof(prod.descricao), descricao, _TRUNCATE);
    strncpy_s(prod.categoria, sizeof(prod.categoria), categoria, _TRUNCATE);
    strncpy_s(prod.codigo_barras, sizeof(prod.codigo_barras), codigo_barras, _TRUNCATE);
    prod.preco = preco;
    prod.quantidade = quantidade;
    return prod;
}

// Função para buscar um produto pelo ID na tabela
TProd TProd_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TProd prod = {0}; // Inicializa com valores padrão
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return prod;
    }
    int index = DB_FindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return prod;
    }
    // binary search
    // Obter os offsets de início e fim da tabela
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    // Calcular o número de registros
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size; // -1
    printf("end: %lu\n", end);
    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        printf("middle: %lu\n", middle);
        size_t seek = start_offset + (middle * size);
        printf("seek: %zu\n", seek);
        fseek(file, seek, SEEK_SET);
        prod = TProd_Read(file);
        if (prod.pk == pk) {
            return prod;
        }
        if (prod.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return prod;
}

TProd TProd_Read(FILE *file) {
    TProd prod = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return prod;
    }
    if (fread(&prod.pk, sizeof(unsigned long), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return prod;
    }
    fread(prod.nome, sizeof(char), sizeof(prod.nome), file);
    fread(prod.descricao, sizeof(char), sizeof(prod.descricao), file);
    fread(prod.categoria, sizeof(char), sizeof(prod.categoria), file);
    fread(prod.codigo_barras, sizeof(char), sizeof(prod.codigo_barras), file);
    fread(&prod.preco, sizeof(double), 1, file);
    fread(&prod.quantidade, sizeof(int), 1, file);
    return prod;
}


void TProd_Print(TProd *prod){
    if (prod == NULL){
        perror("Produto nulo");
        return;
    }
    printf("# Produto:\n");
    printf("| PK: %lu\n", prod->pk);
    printf("| Nome: %s\n", prod->nome);
    printf("| Descricao: %s\n", prod->descricao);
    printf("| Categoria: %s\n", prod->categoria);
    printf("| Código de Barras: %s\n", prod->codigo_barras);
    printf("| Preço: %.2lf\n", prod->preco);
    printf("| Quantidade: %d\n", prod->quantidade);
}