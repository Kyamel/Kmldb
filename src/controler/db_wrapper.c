#include "../kmldb/db.h"
#include <stdio.h>

FILE* cInit(const char *filename) {
    return dbInit(filename);
}

void cClose(FILE *file) {
    dbClose(file);
}

void cWelcome() {
    dbWelcome();
}

int cCreateTable(FILE *file, const char *table_name, size_t size) {
    return dbCreateTable(file, table_name, size);
}

int cAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset) {
    return dbAdd(file, table_name, reg, reg_size, pk_offset);
}