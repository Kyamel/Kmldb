#ifndef DB_WRAPPER_H
#define DB_WRAPPER_H

#include <stdio.h>
#include "../kmldb/db.h"

FILE* cdbInit(const char *filename);

int cdbClose(FILE *file);

void cdbWelcome();

int cdbCreateTable(FILE *file, const char *table_name, size_t size);

int cdbAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, int (*generic_write)(FILE*, void*, size_t));

int cdbFind(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset, size_t next_pk_offset, size_t status_offset);

int cdbUpdate(FILE *file, const char *table_name, unsigned long pk, void *new_reg, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, size_t status_offset, int (*generic_write)(FILE*, void*, size_t));

int cdbDelete(FILE *file, const char *table_name, unsigned long pk, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, size_t status_offset, int (*generic_write)(FILE*, void*, size_t));

int cdbReadAll(FILE* file, const char *table_name, size_t pk_offset, size_t status_offset, size_t reg_size, PrintCallback printFunc);

int cdbReadAllNoHeader(FILE* file, void* member, size_t member_size, PrintCallback printFunc);

#endif