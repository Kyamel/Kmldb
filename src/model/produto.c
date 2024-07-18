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
    prod.pk = header->tables[index].next_pk++;
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
TProd TProd_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
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
    // binary search
    // Obter os offsets de início e fim da tabela
    long start_offset = header->tables[index].start_offset;
    long end_offset = header->tables[index].end_offset;
    // Calcular o número de registros
    long start = 0;
    long end = (end_offset - start_offset) / header->tables[index].size - 1;
    while (start <= end) {
        long middle = (start + end) / 2;
        long middle_offset = start_offset + middle * header->tables[index].size;
        fseek(file, middle_offset, SEEK_SET);
        TProd *p = TProd_Read(file);
        if (p == NULL) {
            break;
        }
        if (p->pk == pk) {
            prod = *p;
            free(p);
            free(header);
            return prod;
        }
        if (p->pk < pk) start = middle + 1;
        else end = middle - 1;
        free(p);
    }
    free(header);
    return prod;
}

TProd *TProd_Read(FILE *file) {
    TProd *prod = (TProd *) malloc(sizeof(TProd));

    if (0 >= fread(&prod->pk, sizeof(long unsigned), 1, file)){
        free(prod);
        return NULL;
    }

    fread(prod->nome, sizeof(char), sizeof(prod->nome), file);
    fread(prod->descricao, sizeof(char), sizeof(prod->descricao), file);
    fread(prod->categoria, sizeof(char), sizeof(prod->categoria), file);
    fread(prod->codigo_barras, sizeof(char), sizeof(prod->codigo_barras), file);
    
    fread(&prod->preco, sizeof(prod->preco), 1, file);  // Corrigido para sizeof(prod->preco)
    fread(&prod->quantidade, sizeof(prod->quantidade), 1, file);  // Corrigido para sizeof(prod->quantidade)
    
    return prod;
}


void TProd_Print(TProd prod){
    printf("# Produto:\n");
    printf("| PK: %lu\n", prod.pk);
    printf("| Nome: %s\n", prod.nome);
    printf("| Descricao: %s\n", prod.descricao);
    printf("| Categoria: %s\n", prod.categoria);
    printf("| Código de Barras: %s\n", prod.codigo_barras);
    printf("| Preço: %.2lf\n", prod.preco);
    printf("| Quantidade: %d\n", prod.quantidade);
}