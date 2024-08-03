#ifndef CRUD_H
#define CRUD_H

#include <stdio.h>

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"


// ########
// #TREINO#
// ########

// Não faz checagem da existeência do cliente e do exercicio
int cAddTreinoDoC(FILE* file, const char* table_name, const char* nome, const char* tipo, long unsigned epk, long unsigned cpk);

// Faz checagem da existeência do cliente e do exercicio
int cAddTreinoNotC(FILE* file, const char* table_name, const char* nome, const char* tipo, TExerc* exerc, TCliente* cliente);

TTreino cReadTreino(FILE* file, const char* table_name, long unsigned pk);

void cPrintTreino(TTreino* treino);

// #############
// #FUNCIONARIO#
// #############

// Adiciona um novo funcionário
int cAddFunc(FILE* file, const char* table_name, const char* nome, const char* cpf, const char* email, const char* tel, const char* data_nascimento, double salario);

TFunc cReadFunc(FILE* file, const char* table_name, long unsigned pk);

void cPrintFunc(TFunc* func);

// ###########
// #EXERCICIO#
// ###########

// Adiciona um novo exercício
int CAddExerc(FILE* file, const char* table_name, const char* nome, const char* tipo, int duration);

TExerc cReadExerc(FILE* file, const char* table_name, long unsigned pk);

void cPrintExerc(TExerc* exec);

// #########
// #CLIENTE#
// #########

// Adiciona um novo cliente
int cAddCliente(FILE* file, const char* table_name, const char* nome, const char *cpf, const char* email, const char* tel, const char* exp_date);

TCliente cReadCliente(FILE* file, const char* table_name, long unsigned pk);

void cPrintCliente(TCliente* cliente);

#endif