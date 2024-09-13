#ifndef TREINO_H
#define TREINO_H

#include "../kmldb/db.h"
#include "../utils/list.h"

#include <stdio.h>

#define BT_NOME 50
#define BT_TIPO 15

typedef struct {
    long unsigned pk; // id autogerado e único
    long unsigned cpk; // referencia o cliente
    long unsigned epk; // referencia o exercicio
    char nome[BT_NOME];
    char tipo[BT_TIPO];
    int duration; // duracao do exercicio em segundos
} TTreino;

int TTreino_Size();

TTreino TTreino_New(unsigned long pk, const char* nome, const char* tipo, long unsigned cpk, long unsigned epk, int duration);

TTreino TTreino_GetByPK(FILE *file, const char* table_name, long unsigned pk);

TTreino TTreino_GetByCpkEpk(FILE *file, const char* table_name, long unsigned cpk, long unsigned epk);

node_t *TTreino_GetByCpk(FILE *file, const char* table_name, long unsigned cpk);

int TTreino_ReadByCpk(FILE *file, const char* table_name, long unsigned cpk);

TTreino TTreino_ReadReg(FILE *file);

void TTreino_Print(TTreino *treino);

void TTreino_PrintGeneric(void* member);

int TTreinoSelecaoComSubstituicaoCpkk(FILE *file, const char *table_name);

int TTreinoIntercalacaoBasicaCpk(FILE *file, DatabaseHeader *header, int num_particions);

int TTreinoClassificacaoInterna(FILE *file, const char* table_name);

int TTreinoIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes);


// ###

int TTreinoSelecaoComSubstituicao(FILE *file, const char *table_name);

int TTreinoIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes);

#endif