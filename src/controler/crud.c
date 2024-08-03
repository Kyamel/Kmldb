#include "../kmldb/db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crud.h"

// ########
// #TREINO#
// ########

// Não faz checagem da existeência do cliente e do exercicio
int cAddTreinoNotC(FILE* file, const char* table_name, const char* nome, const char* tipo, TExerc* exerc, TCliente* cliente) {
    TTreino treino = TTreino_New(nome, tipo, cliente->pk, exerc->pk);

    dbAdd(file, table_name, &treino, sizeof(treino), DB_PK_OFFSET(TTreino));
    return 0;
}

// Faz checagem da existeência do cliente e do exercicio
int cAddTreinoDoC(FILE* file, const char* table_name, const char* nome, const char* tipo, long unsigned epk, long unsigned cpk) {
    TExerc e = TExerc_GetByPK(file, table_name, epk);
    if (e.pk == 0) {
        printf("Exercicio inexistente\n");
        return -1;
    }
    TCliente c =TCliente_GetByPK(file, table_name, cpk);
    if (c.pk == 0) {
        printf("Cliente inexistente\n");
        return -1;
    }
    TTreino treino = TTreino_New(nome, tipo, cpk, epk);
    dbAdd(file, table_name, &treino, sizeof(treino), DB_PK_OFFSET(TTreino));
    return 0;
}

TTreino cReadTreino(FILE* file, const char* table_name, long unsigned pk) {
    TTreino treino = TTreino_GetByPK(file, table_name, pk);
    return treino;
}

void cPrintTreino(TTreino* treino) {
    TTreino_Print(treino);
}

// #############
// #FUNCIONARIO#
// #############

int cAddFunc(FILE* file, const char* table_name, const char* nome, const char* cpf, const char* email, const char* tel, const char* data_nascimento, double salario) {
    TFunc func = TFunc_New(nome, cpf, email, tel, data_nascimento, salario);
    dbAdd(file, table_name, &func, sizeof(func), DB_PK_OFFSET(TFunc));
    return 0;
}

TFunc cReadFunc(FILE* file, const char* table_name, long unsigned pk) {
    TFunc func = TFunc_GetByPK(file, table_name, pk);
    return func;
}

void cPrintFunc(TFunc* func) {
    TFunc_Print(func);
}

// ###########
// #EXERCICIO#
// ###########

int CAddExerc(FILE* file, const char* table_name, const char* nome, const char* tipo, int duration) {
    TExerc exerc = TExerc_New(nome, tipo, duration);
    dbAdd(file, table_name, &exerc, sizeof(exerc), DB_PK_OFFSET(TExerc));
    return 0;
}

TExerc cReadExerc(FILE* file, const char* table_name, long unsigned pk) {
    TExerc exerc = TExerc_GetByPK(file, table_name, pk);
    return exerc;
}

void cPrintExerc(TExerc* exerc) {
    TExerc_Print(exerc);
}

// #########
// #CLIENTE#
// #########

int cAddCliente(FILE* file, const char* table_name, const char* nome, const char *cpf, const char* email, const char* tel, const char* exp_date) {
    TCliente cliente = TCliente_New(nome, cpf, email, tel, exp_date);
    dbAdd(file, table_name, &cliente, sizeof(cliente), DB_PK_OFFSET(TCliente));
    return 0;
}

TCliente cReadCliente(FILE* file, const char* table_name, long unsigned pk) {
    TCliente cliente = TCliente_GetByPK(file, table_name, pk);
    return cliente;
}

void cPrintCliente(TCliente* cliente) {
    TCliente_Print(cliente);
}