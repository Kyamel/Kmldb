#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>

#define BE_NOME 50
#define BE_TIPO 20

typedef struct {
    long unsigned pk; // id autogerado e único
    char nome[BE_NOME];
    char tipo[BE_TIPO];
    int duration; // segundos
} TExerc;

int TExerc_Size();

TExerc TExerc_New(unsigned long pk, const char* nome, const char* tipo, int duration);

TExerc TExerc_GetByPK(FILE *file, const char* table_name, long unsigned pk);

TExerc TExerc_Read(FILE *file);

void TExerc_Print(TExerc *exec);

#endif