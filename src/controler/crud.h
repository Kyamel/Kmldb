#ifndef CRUD_H
#define CRUD_H

#include <stdio.h>

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

#define DB_FOLDER "./data"
#define CLIENTES "clientes"
#define FUNCIONARIOS "funcionarios"
#define TREINOS "treinos"
#define EXERCICIOS "exercicios"

// ###################
// Funções Auxiliares#
// ###################

void cInitTables(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexec);

int cCloseDatabase(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer);

// ########
// #TREINO#
// ########

// Não faz checagem da existeência do cliente e do exercicio
int cAddTreinoDoC(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* tipo, long unsigned epk, long unsigned cpk, int duration);

// Faz checagem da existeência do cliente e do exercicio
int cAddTreinoNotC(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* tipo, TExerc* exerc, TCliente* cliente, int duration);

TTreino cSearchTreino(FILE* file, const char* table_name, long unsigned pk);

TTreino cSearchTreinoByCpkEpk(FILE* file, const char* table_name, long unsigned cpk, long unsigned epk);

// imprime os dados do exercício
int cSearchPrintTreinoFullByCpk(FILE* ftreino, FILE* fexerc, long unsigned epk);

// não imprime os dados do exercício
int cSearchPrintTreinoByCpk(FILE* file, const char* table_name, long unsigned cpk);

void cPrintTreino(TTreino* treino);

// #############
// #FUNCIONARIO#
// #############

// Adiciona um novo funcionário
int cAddFunc(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* cpf, const char* email, const char* tel, const char* data_nascimento, double salario);

TFunc cSearchFunc(FILE* file, const char* table_name, long unsigned pk);

void cPrintFunc(TFunc* func);

// ###########
// #EXERCICIO#
// ###########

// Adiciona um novo exercício
int cAddExerc(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char* tipo, int duration);

TExerc cSearchExerc(FILE* file, const char* table_name, long unsigned pk);

void cPrintExerc(TExerc* exec);

// #########
// #CLIENTE#
// #########

// Adiciona um novo cliente
int cAddCliente(FILE* file, const char* table_name, unsigned long pk, const char* nome, const char *cpf, const char* email, const char* tel, const char* exp_date);

TCliente cSearchCliente(FILE* file, const char* table_name, long unsigned pk);

void cPrintCliente(TCliente* cliente);

#endif