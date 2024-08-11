#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#include "../kmldb/db.h"

#include <stdio.h>

#define BF_NOME 50
#define BF_CPF 15
#define BF_DATA_NASCIMENTO 11
#define BF_EMAIL 50
#define BF_TELEFONE 15

typedef struct {
    long unsigned pk; // id autogerado e único
    char nome[BF_NOME];
    char cpf[BF_CPF];
    char email[BF_EMAIL];
    char telefone[BF_TELEFONE];
    char data_nascimento[BF_DATA_NASCIMENTO];
    double salario; // salário do funcionário
} TFunc;

int TFunc_Size();

TFunc TFunc_New(unsigned long pk, const char* nome, const char* cpf, const char* email, const char* telefone, const char* data_nascimento, double salario);

TFunc TFunc_GetByPK(FILE *file, const char* table_name, long unsigned pk);

void TFunc_Print(TFunc *func);

TFunc TFunc_ReadReg(FILE *file);

void TFunc_PrintGeneric(void* member);

int TFuncIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particions);

int TFuncClassificacaoInterna(FILE *file, const char* table_name);

#endif // FUNCIONARIO_H
