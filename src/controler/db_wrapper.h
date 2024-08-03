#ifndef DB_WRAPPER_H
#define DB_WRAPPER_H

#include <stdio.h>

FILE* cdbInit(const char *filename);

void cdbClose(FILE *file);

void cdbWelcome();

int cdbCreateTable(FILE *file, const char *table_name, size_t size);

int cdbAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset);

#endif