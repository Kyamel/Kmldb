#ifndef EXEC_H
#define EXEC_H

#include "../kmldb/db.h"

#include <stdio.h>

#define BE_NOME 50
#define BE_TIPO 20

typedef struct {
    long unsigned pk; // id autogerado e único
    size_t next_pk; // próximo índice na lista encadeada
    int status; // status do funcionário
    char nome[BE_NOME];
    char tipo[BE_TIPO];
    int duration; // segundos
} TExerc;

int TExerc_Size();

TExerc TExerc_New(unsigned long pk, const char* nome, const char* tipo, int duration, size_t next_pk, int status);

TExerc TExerc_GetByPK(FILE *file, const char* table_name, long unsigned pk);

TExerc TExerc_ReadReg(FILE *file);

void TExerc_Print(TExerc *exec);

void TExerc_PrintGeneric(void* member);

int TExercClassificacaoInterna(FILE *file, const char* table_name);

int TExercIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particions);


// ###

int TExercSelecaoComSubstituicao(FILE *file, const char *table_name);

int TExercIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes);

#endif