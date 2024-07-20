#include <stddef.h>
#include <stdio.h>

#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

#define B_NOME 50
#define B_CPF 15
#define B_DATA_NASCIMENTO 11

typedef struct {
    long unsigned pk; // id autogerado e único
    char nome[B_NOME];
    char cpf[B_CPF];
    char data_nascimento[B_DATA_NASCIMENTO];
    double salario;
} TFunc;

TFunc TFunc_New(const char* nome, const char* cpf, const char* data_nascimento, double salario);

TFunc TFunc_GetByPK(FILE *file, const char* table_name, long unsigned pk);

void TFunc_Print(TFunc *func);

TFunc TFunc_Read(FILE *(in));

int TFunc_Size();

#endif // FUNCIONARIO_H
