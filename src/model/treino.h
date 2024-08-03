#ifndef TREINO_H
#define TREINO_H

#include <stdio.h>

#define BT_NOME 50
#define BT_TIPO 15

typedef struct {
    long unsigned pk; // id autogerado e único
    long unsigned cpk; // referencia o cliente
    long unsigned epk; // referencia o exercicio
    char nome[BT_NOME];
    char tipo[BT_TIPO];
} TTreino;

int TTreino_Size();

TTreino TTreino_New(const char* nome, const char* tipo, long unsigned cpk, long unsigned epk);

TTreino TTreino_GetByPK(FILE *file, const char* table_name, long unsigned pk);

TTreino TTreino_Read(FILE *file);

void TTreino_Print(TTreino *treino);

#endif