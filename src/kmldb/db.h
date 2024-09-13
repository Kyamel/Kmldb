#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DB_H
#define DB_H

#define B_TABLE_NAME 20
#define TABLE_MAX_COUNT 10

// Macro para verificar o offset do campo pk
#define DB_CHECK_PK_OFFSET(type) assert(offsetof(type, pk) == 0)
#define DB_PK_OFFSET(type) offsetof(type, pk)

typedef struct {
    char table_name[B_TABLE_NAME];
    long unsigned next_pk;
    long unsigned start_offset;
    long unsigned end_offset;
    size_t size;
} TableMeta;

typedef struct {
    int table_count;
    TableMeta tables[TABLE_MAX_COUNT]; // Suportamos até 10 tabelas
} DatabaseHeader;

// Callback para impressão
typedef void (*PrintCallback)(void*);

// Um ERR é definido por um inteiro negativo
// 0 indica que não houve erro
// A primeira casa indica a origem do erro
// A segunda indica o tipo de erro
// A terceira indica o erros associados a bibliotecas C (fwrite, fread, etc)

// REGISTER 1
// TABLE 2
// HEADER 3
// DB 4

// NOT_FOUND 1
// EXISTS 2
// INCOMPATIBLE_SIZE 3
// INVALID_STRUCT 4
// MAX_SIZE_EXCEEDED 5
// INVALID_NAME 6

// READ_FAILED 1
// WRITE_FAILED 2
// CLOSE_FAILED 3
// OPEN_FAILED 4

#define ERR_OPEN_FAILED -004

#define DB_OK 0
#define ERR_DB_CLOSE_FAILED -403
#define ERR_DB_OPEN_FAILED -404

#define ERR_REGISTER_NOT_FOUND -110
#define ERR_REGISTER_EXISTS -120
#define ERR_REGISTER_INCOMPATIBLE_SIZE -130
#define ERR_REGISTER_INVALID_STRUCT -140
#define ERR_REGISTER_READ_FAILED -101
#define ERR_REGISTER_WRITE_FAILED -102

#define ERR_TABLE_NOT_FOUND -210
#define ERR_TABLE_EXISTS -220
#define ERR_TABLE_READ_FAILED -201
#define ERR_TABLE_WRITE_FAILED -202
#define ERR_TABLE_MAX_SIZE_EXCEEDED -250
#define ERR_TABLE_INVALID_NAME -260

#define ERR_HEADER_NOT_FOUND -310
#define ERR_HEADER_EXISTS -320
#define ERR_HEADER_READ_FAILED -301
#define ERR_HEADER_WRITE_FAILED -302

// Inicializa o Banco de Dados
FILE* dbInit(const char* filename);

// Encerra o Banco de Dados
int dbClose(FILE* file);

void dbWelcome();

// Procura uma tabela no Banco de Dados, retorna -1 se a tabela não existir
int dbFindTable(FILE* file, const char* table_name);

// Cria uma tabela no Banco de Dados
int dbCreateTable(FILE* file, const char* table_name, size_t size);

// Imprime o cabeçalho do Banco de Dados
void dbPrintHeader(DatabaseHeader* header);

// Insere novo registro na tabela indicada
int dbAdd(FILE* file, const char* table_name, void* member, size_t member_size, size_t pk_offset);

int dbReadAll(FILE* file, const char* table_name, void* member, size_t member_size, PrintCallback printFunc);

int dbReadAllNoHeader(FILE* file, void* member, size_t member_size, PrintCallback printFunc);

void dbAddMember2(FILE* file, const char* table_name, void* member, size_t member_size, size_t pk_offset);

#endif