#include <stdio.h>

#ifndef PRODUTO_H
#define PRODUTO_H

#define B_NOME 50
#define B_DESCRICAO 200 // Estranho, da erro na leitura do Produto se coloco 500 ou 100
#define B_CATEGORIA 30
#define B_CODIGO_BARRAS 20

typedef struct {
    long unsigned pk; // id autogerado e único
    char nome[B_NOME];
    char descricao[B_DESCRICAO];
    char categoria[B_CATEGORIA];
    char codigo_barras[B_CODIGO_BARRAS];
    double preco;
    int quantidade;
} TProd;

int TProd_Size();

TProd TProd_New(const char* nome, const char* descricao, const char* categoria, const char* codigo_barras, double preco, int quantidade);

TProd TProd_GetByPK(FILE *file, const char* table_name, long unsigned pk);

void TProd_Print(TProd *prod);

TProd TProd_Read(FILE *(in));

#endif