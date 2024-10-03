#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#include "../kmldb/db.h"

#include <stdio.h>

#define BF_NOME 50
#define BF_CPF 15
#define BF_DATA_NASCIMENTO 11
#define BF_EMAIL 50
#define BF_TELEFONE 15
#define HASH_SIZE 30

typedef struct {
    long unsigned pk; // id autogerado e único
    size_t next_pk; // próximo índice na lista encadeada
    double salario; // salário do funcionário
    int status; // status do funcionário
    char data_nascimento[BF_DATA_NASCIMENTO]; // data de nascimento do funcionário
    char cpf[BF_CPF]; // CPF do funcionário
    char telefone[BF_TELEFONE]; // telefone do funcionário
    char password[HASH_SIZE]; // hash da senha
    char nome[BF_NOME]; // nome do funcionário
    char email[BF_EMAIL]; // email do funcionário
} TFunc;

int TFunc_Size();

TFunc TFunc_New(unsigned long pk, const char* nome, const char* cpf, const char* email,
    const char* telefone, const char* data_nascimento, double salario,
    const char* password, int status, size_t next_pk);

TFunc TFunc_GetByPK(FILE *file, const char* table_name, long unsigned pk);

void TFunc_Print(TFunc *func);

TFunc TFunc_ReadReg(FILE *file);

void TFunc_PrintGeneric(void* member);

int TFuncIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particions);

int TFuncClassificacaoInterna(FILE *file, const char* table_name);

// ###

int TFuncSelecaoComSubstituicao(FILE *file, const char *table_name);

int TFuncIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes);

// #####################################

int TFunc_HashAdd(FILE* file, const char* table_name, TFunc member);


#endif // FUNCIONARIO_H
