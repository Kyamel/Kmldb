#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../kmldb/db.h"
#include "../utils/util.h"

#include "crud.h"
#include "db_wrapper.h"

void cInitTables(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexec) {
    cdbCreateTable(fcli, CLIENTES, sizeof(TCliente));
    cdbCreateTable(ffunc, FUNCIONARIOS, sizeof(TFunc));
    cdbCreateTable(ftreino, TREINOS, sizeof(TTreino));
    cdbCreateTable(fexec, EXERCICIOS, sizeof(TExerc));
}

int cCloseDatabase(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {
    int ok;
    printf("Cliente ");
    ok = cdbClose(fcli);
    if (ok < 0) perror("Erro ao fechar o arquivo clientes.dat\n");
    printf("Funcionario ");
    ok = cdbClose(ffunc);
    if (ok < 0) perror("Erro ao fechar o arquivo funcionarios.dat\n");
    printf("Treino ");
    ok = cdbClose(ftreino);
    if (ok < 0) perror("Erro ao fechar o arquivo treinos.dat\n");
    printf("Exercicio ");
    ok = cdbClose(fexer);
    if (ok < 0) perror("Erro ao fechar o arquivo exercicios.dat\n");
    printf("Saindo... Pressione ENTER\n");
    cClearInputBuffer();
    return ok;
}

// ########
// #TREINO#
// ########

// Não faz checagem da existeência do cliente e do exercicio
int cAddTreinoNotC(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* tipo, TExerc* exerc, TCliente* cliente) {
    TTreino treino = TTreino_New(pk, nome, tipo, cliente->pk, exerc->pk);

    dbAdd(file, table_name, &treino, sizeof(treino), DB_PK_OFFSET(TTreino));
    return 0;
}

// Faz checagem da existeência do cliente e do exercicio
int cAddTreinoDoC(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* tipo, long unsigned epk, long unsigned cpk) {
    TExerc e = TExerc_GetByPK(file, EXERCICIOS, epk);
    if (e.pk == 0) {
        printf("Exercicio inexistente\n");
        return -1;
    }
    TCliente c =TCliente_GetByPK(file, CLIENTES, cpk);
    if (c.pk == 0) {
        printf("Cliente inexistente\n");
        return -1;
    }
    TTreino treino = TTreino_New(pk, nome, tipo, cpk, epk);
    dbAdd(file, table_name, &treino, sizeof(treino), DB_PK_OFFSET(TTreino));
    return 0;
}

TTreino cSearchTreino(FILE* file, const char* table_name, long unsigned pk) {
    TTreino treino = TTreino_GetByPK(file, table_name, pk);
    return treino;
}

TTreino cSearchTreinoComp(FILE* file, const char* table_name, long unsigned cpk, long unsigned epk) {
    TTreino treino = TTreino_GetByCpkEpk(file, table_name, cpk, epk);
    return treino;
}

void cPrintTreino(TTreino* treino) {
    TTreino_Print(treino);
}

// #############
// #FUNCIONARIO#
// #############

int cAddFunc(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* cpf, const char* email, const char* tel, const char* data_nascimento, double salario) {
    TFunc func = TFunc_New(pk,nome, cpf, email, tel, data_nascimento, salario);
    dbAdd(file, table_name, &func, sizeof(func), DB_PK_OFFSET(TFunc));
    return 0;
}

TFunc cSearchFunc(FILE* file, const char* table_name, long unsigned pk) {
    TFunc func = TFunc_GetByPK(file, table_name, pk);
    return func;
}

void cPrintFunc(TFunc* func) {
    TFunc_Print(func);
}

// ###########
// #EXERCICIO#
// ###########

int cAddExerc(FILE* file, const char* table_name,  unsigned long pk, const char* nome, const char* tipo, int duration) {
    TExerc exerc = TExerc_New(pk, nome, tipo, duration);
    dbAdd(file, table_name, &exerc, sizeof(exerc), DB_PK_OFFSET(TExerc));
    return 0;
}

TExerc cSearchExerc(FILE* file, const char* table_name, long unsigned pk) {
    TExerc exerc = TExerc_GetByPK(file, table_name, pk);
    return exerc;
}

void cPrintExerc(TExerc* exerc) {
    TExerc_Print(exerc);
}

// #########
// #CLIENTE#
// #########

int cAddCliente(FILE* file, const char* table_name,  unsigned long pk, const char* nome, const char *cpf, const char* email, const char* tel, const char* exp_date) {
    TCliente cliente = TCliente_New(pk, nome, cpf, email, tel, exp_date);
    dbAdd(file, table_name, &cliente, sizeof(cliente), DB_PK_OFFSET(TCliente));
    return 0;
}

TCliente cSearchCliente(FILE* file, const char* table_name, long unsigned pk) {
    TCliente cliente = TCliente_GetByPK(file, table_name, pk);
    return cliente;
}

void cPrintCliente(TCliente* cliente) {
    TCliente_Print(cliente);
}