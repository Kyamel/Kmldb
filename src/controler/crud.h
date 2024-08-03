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
int addTreinoDoC(FILE* file, const char* table_name, const char* nome, const char* tipo, int epk, int cpk);

// Faz checagem da existeência do cliente e do exercicio
int addTreinoNotC(FILE* file, const char* table_name, const char* nome, const char* tipo, TExerc* exerc, TCliente* cliente);

TTreino readTreino(FILE* file, const char* table_name);

void printTreino(TTreino* treino);

// #############
// #FUNCIONARIO#
// #############

// Adiciona um novo funcionário
int addFunc(FILE* file, const char* table_name, const char* nome, const char* cpf, const char* email, const char* tel, const char* data_nascimento, double salario);

TFunc readFunc(FILE* file, const char* table_name);

void printFunc(TFunc* func);

// ###########
// #EXERCICIO#
// ###########

// Adiciona um novo exercício
int addExerc(FILE* file, const char* table_name, const char* nome, const char* tipo, int duration);

TExerc readExerc(FILE* file, const char* table_name);

void printExerc(TExerc* exec);

// #########
// #CLIENTE#
// #########

// Adiciona um novo cliente
int addCliente(FILE* file, const char* table_name, const char* nome, const char *cpf, const char* email, const char* tel, const char* exp_date);

TCliente readCliente(FILE* file, const char* table_name);

void printCliente(TCliente* cliente);

#endif