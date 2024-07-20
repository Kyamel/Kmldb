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

FILE* DB_Init(const char* filename);

void DB_Welcome();

int DB_FindTable(FILE* file, const char* table_name);

void DB_CreateTable(FILE* file, const char* table_name, size_t size);

void DB_PrintHeader(DatabaseHeader* header);

void DB_AddMember(FILE* file, const char* table_name, void* member, size_t member_size, size_t offset);

#endif