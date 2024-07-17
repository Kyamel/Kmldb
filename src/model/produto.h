#include <stdio.h>

#ifndef PRODUTO_H
#define PRODUTO_H

#define B_NOME 50
#define B_DESCRICAO 500
#define B_CATEGORIA 30
#define B_CODIGO_BARRAS 20

typedef struct {
    int id; // id autogerado e único
    char nome[B_NOME];
    char descricao[B_DESCRICAO];
    char categoria[B_CATEGORIA];
    char codigo_barras[B_CODIGO_BARRAS];
    double preco;
    int quantidade;
} TProd;

int TProd_Size();

TProd TProd_New(FILE *file, const char* table_name, const char* nome, const char* descricao, const char* categoria, const char* codigo_barras, double preco, int quantidade);

TProd TProd_GetByPK(FILE *file, const char* table_name, int id);

void TProd_Print(TProd prod);

TProd *TProd_Read(FILE *(in));

#endif