#include "../kmldb/db.h"
#include <stdio.h>

FILE* cdbInit(const char *filename) {
    return dbInit(filename);
}

int cdbClose(FILE *file) {
    return dbClose(file);
}

void cdbWelcome() {
    dbWelcome();
}

int cdbCreateTable(FILE *file, const char *table_name, size_t size) {
    return dbCreateTable(file, table_name, size);
}

int cdbAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, int (*generic_write)(FILE*, void*, size_t)) {
    return dbHashAdd(file, table_name, reg, reg_size, pk_offset, next_pk_offset, generic_write);
}

int cdbFind(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset, size_t next_pk_offset, size_t status_offset) {
    return dbHashFind(file, table_name, reg, reg_size, pk_offset, next_pk_offset, status_offset);
}

int cdbUpdate(FILE *file, const char *table_name, unsigned long pk, void *new_reg, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, size_t status_offset, int (*generic_write)(FILE*, void*, size_t)) {
    return dbHashUpdate(file, table_name, pk, new_reg, reg_size, pk_offset, next_pk_offset, status_offset, generic_write);
}

int cdbDelete(FILE *file, const char *table_name, unsigned long pk, size_t reg_size, size_t pk_offset,
    size_t next_pk_offset, size_t status_offset, int (*generic_write)(FILE*, void*, size_t)) {
    return dbHashDelete(file, table_name, pk, reg_size, pk_offset, next_pk_offset, status_offset, generic_write);
}

int cdbReadAll(FILE* file, const char *table_name, size_t pk_offset, size_t status_offset, size_t reg_size, PrintCallback printFunc){
    return dbPrintTable(file, table_name, pk_offset, status_offset, reg_size, printFunc);
}

int cdbReadAllNoHeader(FILE* file, void* member, size_t member_size, PrintCallback printFunc){
    return dbReadAllNoHeader(file, member, member_size, printFunc);
}
