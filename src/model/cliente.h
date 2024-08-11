#ifndef CLIENTE_H
#define CLIENTE_H

#include "../kmldb/db.h"

#include <stdio.h>

#define BC_NOME 50
#define BC_CPF 15
#define BC_EMAIL 50
#define BC_EXP_DATE 11
#define BC_TELEFONE 15

typedef struct {
    long unsigned pk; // id autogerado e único
    char nome[BC_NOME];
    char cpf[BC_CPF];
    char email[BC_NOME];
    char telefone[BC_TELEFONE];
    char exp_date[BC_EXP_DATE]; // baseado no pagamento, a data de exp é gerada
} TCliente;

int TCliente_Size();

TCliente TCliente_New(unsigned long pk, const char* nome, const char* cpf, const char* email, const char* telefone, const char* exp_date);

TCliente TCliente_GetByPK(FILE *file, const char* table_name, long unsigned pk);

TCliente TCliente_ReadReg(FILE *file);

void TCliente_Print(TCliente *cliente);

void TCliente_PrintGeneric(void* member);

int TClienteIntercalacaoBasicaTeste(FILE *file, DatabaseHeader *header, int num_particions);

int TClienteClassificacaoInterna(FILE *file, const char* table_name);

int TClienteIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes);


#endif