#include <stddef.h>
#include <stdio.h>

#ifndef DB_H
#define DB_H

#define B_TABLE_NAME 20
#define TABLE_MAX_COUNT 10

typedef struct {
    char table_name[B_TABLE_NAME];
    int next_id;
    long start_offset;
    long end_offset;
} TableMeta;

typedef struct {
    int table_count;
    TableMeta tables[TABLE_MAX_COUNT]; // Suportamos até 10 tabelas
} DatabaseHeader;

FILE* DB_Init(const char* filename);

void DB_Welcome();

int DB_FindTable(FILE* file, const char* table_name);

void DB_CreateTable(FILE* file, const char* table_name);

DatabaseHeader *DB_LoadHeader(FILE *file);

void DB_PrintHeader(DatabaseHeader* header);

void DB_AddMember(FILE* file, const char* table_name, void* member, size_t member_size);

#endif