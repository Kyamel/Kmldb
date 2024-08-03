#ifndef DB_WRAPPER_H
#define DB_WRAPPER_H

#include <stdio.h>

FILE* cInit(const char *filename);

void cClose(FILE *file);

void cWelcome();

int cCreateTable(FILE *file, const char *table_name, size_t size);

int cAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset);

#endif