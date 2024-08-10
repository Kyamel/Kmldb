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

int cdbAdd(FILE *file, const char *table_name, void *reg, size_t reg_size, size_t pk_offset) {
    return dbAdd(file, table_name, reg, reg_size, pk_offset);
}

int cdbReadAll(FILE* file, const char *table_name, void* member, size_t member_size, PrintCallback printFunc){
    return dbReadAll(file, table_name, member, member_size, printFunc);
}

int cdbReadAllNoHeader(FILE* file, void* member, size_t member_size, PrintCallback printFunc){
    return dbReadAllNoHeader(file, member, member_size, printFunc);
}
