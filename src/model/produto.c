#include "produto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

int TProd_Size() {
    return sizeof(int)
          + sizeof(char) * B_NOME
          + sizeof(char) * B_DESCRICAO
          + sizeof(char) * B_CATEGORIA
          + sizeof(char) * B_CODIGO_BARRAS
          + sizeof(double)
          + sizeof(int);
}

TProd TProd_New(FILE* file, const char* table_name, const char* nome, const char* descricao, const char* categoria, const char* codigo_barras, double preco, int quantidade) {
    TProd prod = {0};
    if (strlen(nome) >= B_NOME || strlen(descricao) >= B_DESCRICAO || strlen(categoria) >= B_CATEGORIA || strlen(codigo_barras) >= B_CODIGO_BARRAS) {
        perror("TProd buffer overflow");
        return prod;
    }
    // Carregar o cabeçalho do banco de dados
    DatabaseHeader* header = DB_LoadHeader(file);
    if (header == NULL) {
        perror("Erro ao carregar o cabeçalho do banco de dados");
        return prod;
    }
    // Encontrar a tabela pelo nome
    int index = DB_FindTable(file, table_name);
    // Verificar se a tabela foi encontrada
    if (index == -1) {
        perror("Tabela não encontrada");
        free(header);
        return prod;
    }
    // Atribuir o próximo ID disponível da tabela
    prod.id = header->tables[index].next_id++;
    // Copiar os dados para a estrutura TProd
    strncpy(prod.nome, nome, B_NOME);
    prod.nome[B_NOME - 1] = '\0';        
    strncpy(prod.descricao, descricao, B_DESCRICAO);
    prod.descricao[B_DESCRICAO - 1] = '\0';
    strncpy(prod.categoria, categoria, B_CATEGORIA);
    prod.categoria[B_CATEGORIA - 1] = '\0';
    strncpy(prod.codigo_barras, codigo_barras, B_CODIGO_BARRAS);
    prod.codigo_barras[B_CODIGO_BARRAS - 1] = '\0';
    prod.preco = preco;
    prod.quantidade = quantidade;
    // Reposiciona para escrever o cabeçalho de volta no arquivo
    fseek(file, 0, SEEK_SET);
    fwrite(header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
    // Liberar a memória do cabeçalho carregado
    free(header);
    return prod;
}

// Função para buscar um produto pelo ID na tabela
TProd TProd_GetByPK(FILE *file, const char* table_name, int id) {
    TProd prod = {0}; // Inicializa com valores padrão
    DatabaseHeader *header = DB_LoadHeader(file);
    if (header == NULL) {
        perror("Erro ao carregar o cabeçalho do banco de dados");
        exit(EXIT_FAILURE);
    }
    int index = DB_FindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return prod;
    }
    // Posicionar no início da tabela
    fseek(file, header->tables[index].start_offset, SEEK_SET);
    TProd *p = NULL;
    while (ftell(file) < header->tables[index].end_offset) {
        if (p != NULL) {
            free(p);
        }
        p = TProd_Read(file);
        if (p->id == id) {
            prod = *p;
            free(p);
            free(header);
            return prod;
        }
    }
    free(p);  // Libera o último alocado, se houver
    free(header);
    return prod;
}

TProd *TProd_Read(FILE *(file)) {
    TProd *prod = (TProd *) malloc(sizeof(TProd));

    if (0 >= fread(&prod->id, sizeof(int), 1, file)){
        free(prod);
        return NULL;
    }

    fread(prod->nome, sizeof(char), sizeof(prod->nome), file);
    fread(prod->descricao, sizeof(char), sizeof(prod->descricao), file);
    fread(prod->categoria, sizeof(char), sizeof(prod->categoria), file);
    fread(prod->codigo_barras, sizeof(char), sizeof(prod->codigo_barras), file);
    fread(&prod->preco, sizeof(double), 1, file);
    fread(&prod->quantidade, sizeof(int), 1, file);
    return prod;

}

void TProd_Print(TProd prod){
    printf("# Produto:\n");
    printf("| id: %d\n", prod.id);
    printf("| nome: %s\n", prod.nome);
    printf("| descricao: %s\n", prod.descricao);
    printf("| categoria: %s\n", prod.categoria);
    printf("| codigo_barras: %s\n", prod.codigo_barras);
    printf("| preco: %lf\n", prod.preco);
    printf("| quantidade: %d\n", prod.quantidade);
}