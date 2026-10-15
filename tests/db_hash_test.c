#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "db.h"

typedef struct {
    unsigned long pk;
    size_t next_pk;
    int status;
    int value;
} TestRow;

int main(void) {
    FILE *file = tmpfile();
    assert(file != NULL);
    DatabaseHeader header = {0};
    assert(fwrite(&header, sizeof header, 1, file) == 1);
    assert(dbCreateTable(file, "rows", sizeof(TestRow)) == DB_OK);

    TestRow first = {0, 0, 0, 11};
    TestRow collision = {32, 0, 0, 22};
    TestRow other_bucket = {2, 0, 0, 33};
    assert(dbHashAdd(file, "rows", &first, sizeof first,
                     offsetof(TestRow, pk), offsetof(TestRow, next_pk), generic_write) == DB_OK);
    assert(first.pk == 1);
    assert(dbHashAdd(file, "rows", &collision, sizeof collision,
                     offsetof(TestRow, pk), offsetof(TestRow, next_pk), generic_write) == DB_OK);
    assert(dbHashAdd(file, "rows", &other_bucket, sizeof other_bucket,
                     offsetof(TestRow, pk), offsetof(TestRow, next_pk), generic_write) == DB_OK);
    rewind(file);
    assert(fread(&header, sizeof header, 1, file) == 1);
    assert(header.tables[0].next_pk == 33);
    assert(header.tables[0].qtd == 3);

    TestRow found = {.pk = 1};
    assert(dbHashFind(file, "rows", &found, sizeof found,
                      offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                      offsetof(TestRow, status)) == DB_OK && found.value == 11);
    found.pk = 32;
    assert(dbHashFind(file, "rows", &found, sizeof found,
                      offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                      offsetof(TestRow, status)) == DB_OK && found.value == 22);
    found.pk = 2;
    assert(dbHashFind(file, "rows", &found, sizeof found,
                      offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                      offsetof(TestRow, status)) == DB_OK && found.value == 33);

    assert(dbHashDelete(file, "rows", 32, sizeof(TestRow),
                        offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                        offsetof(TestRow, status), generic_write) == DB_OK);
    found.pk = 32;
    assert(dbHashFind(file, "rows", &found, sizeof found,
                      offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                      offsetof(TestRow, status)) == EXEPTION_REG_DELETED);
    assert(dbHashDelete(file, "rows", 32, sizeof(TestRow),
                        offsetof(TestRow, pk), offsetof(TestRow, next_pk),
                        offsetof(TestRow, status), generic_write) == EXEPTION_REG_DELETED);
    rewind(file);
    assert(fread(&header, sizeof header, 1, file) == 1);
    assert(header.tables[0].qtd == 2);
    assert(fclose(file) == 0);
    return 0;
}
