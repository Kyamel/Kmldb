#include <time.h>

#include "kmldb/db.h"
#include "model/funcionario.h"
#include "utils/createBase.h"

void run_tests() {
    FILE *funcFile = dbInit("funcionarios_testes.bin");
    dbCreateTable(funcFile, "funcionarios", sizeof(TFunc));

    // Definindo os offsets
    size_t pk_offset = offsetof(TFunc, pk);
    size_t next_pk_offset = offsetof(TFunc, next_pk);
    size_t status_offset = offsetof(TFunc, status);

    // Adição de registros
    printf("Testando adição de registros...\n");

    TFunc f1 = TFunc_New(1, "Ana", "11111111111", "ana@email.com",
                         "11987654321", "01/01/1990", 2500.00, 
                         "senha123", 0, 0);

    TFunc f2 = TFunc_New(2, "Lucas", "22222222222", "lucas@email.com",
                         "11987654322", "02/02/1991", 3000.00,
                         "senha456", 0, 0);

    TFunc f3 = TFunc_New(1, "Maria", "33333333333", "maria@email.com",
                         "11987654323", "03/03/1992", 3500.00,
                         "senha789", 0, 0);

    // Adicionar f1 (ok)
    int err = dbHashAdd(funcFile, "funcionarios", &f1, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
    printf("Adição f1: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);

    // Adicionar f2 (ok)
    err = dbHashAdd(funcFile, "funcionarios", &f2, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
    printf("Adição f2: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);

    // Adicionar f3 com PK repetida (erro esperado)
    err = dbHashAdd(funcFile, "funcionarios", &f3, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
    printf("Adição f3 com PK duplicada: %s (código de erro: %d)\n", err == DB_OK ? "FALHA" : "SUCESSO (ERRO ESPERADO)", err);

    // Leitura de registros
    printf("\nTestando leitura de registros...\n");

    // Ler f1
    TFunc r1;
    r1.pk = 1;
    err = dbHashRead(funcFile, "funcionarios", &r1, sizeof(TFunc), pk_offset, next_pk_offset);
    printf("Leitura f1: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);
    if (err == DB_OK) TFunc_PrintGeneric(&r1);

    // Ler f2
    TFunc r2;
    r2.pk = 2;
    err = dbHashRead(funcFile, "funcionarios", &r2, sizeof(TFunc), pk_offset, next_pk_offset);
    printf("Leitura f2: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);
    if (err == DB_OK) TFunc_PrintGeneric(&r2);

    // Tentar ler um registro inexistente (PK 99)
    TFunc r3;
    r3.pk = 99;
    err = dbHashRead(funcFile, "funcionarios", &r3, sizeof(TFunc), pk_offset, next_pk_offset);
    printf("Leitura PK inexistente: %s (código de erro: %d)\n", err == DB_OK ? "FALHA" : "SUCESSO (ERRO ESPERADO)", err);

    // Atualização de registros
    printf("\nTestando atualização de registros...\n");

    // Atualizar f1
    TFunc f1_updated = TFunc_New(1, "Ana Silva", "11111111111", "ana.silva@email.com",
                                 "11987654324", "01/01/1990", 2700.00, 
                                 "senhaNova123", 0, 0);
    err = dbHashUpdate(funcFile, "funcionarios", f1_updated.pk, &f1_updated, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_write);
    printf("Atualização f1: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);

    // Tentar atualizar PK inexistente
    TFunc f99_updated = TFunc_New(99, "Inexistente", "99999999999", "naoexiste@email.com", 
                                  "11999999999", "09/09/1999", 0, 
                                  "senhaInvalida", 0, 0);
    err = dbHashUpdate(funcFile, "funcionarios", f99_updated.pk, &f99_updated, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_write);
    printf("Atualização PK inexistente: %s (código de erro: %d)\n", err == DB_OK ? "FALHA" : "SUCESSO (ERRO ESPERADO)", err);

    // Deleção de registros
    printf("\nTestando deleção de registros...\n");

    // Deletar f2
    err = dbHashDelete(funcFile, "funcionarios", 2, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_write);
    printf("Deleção f2: %s (código de erro: %d)\n", err == DB_OK ? "SUCESSO" : "FALHA", err);

    // Tentar deletar PK inexistente
    err = dbHashDelete(funcFile, "funcionarios", 99, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_write);
    printf("Deleção PK inexistente: %s (código de erro: %d)\n", err == DB_OK ? "FALHA" : "SUCESSO (ERRO ESPERADO)", err);

    // Tentar ler um registro deletado (f2)
    TFunc r2_deleted;
    r2_deleted.pk = 2;
    err = dbHashRead(funcFile, "funcionarios", &r2_deleted, sizeof(TFunc), pk_offset, next_pk_offset);
    printf("Leitura registro deletado (f2): %s (código de erro: %d)\n", err == DB_OK ? "FALHA" : "SUCESSO (ERRO ESPERADO)", err);

    dbClose(funcFile);
}


void run_tests2(){
    FILE *funcFile = dbInit("funcionarios_testes.bin");
    dbCreateTable(funcFile, "funcionarios", sizeof(TFunc));

    // Definindo os offsets
    size_t pk_offset = offsetof(TFunc, pk);
    size_t next_pk_offset = offsetof(TFunc, next_pk);
    size_t status_offset = offsetof(TFunc, status);

    // Adição de registros
    printf("Adicionando registros...\n");

    TFunc f1 = TFunc_New(1, "Ana", "11111111111", "ana@email.com",
                         "11987654321", "01/01/1990", 2500.00,
                         "senha123", 0, 0);

    TFunc f2 = TFunc_New(63, "Lucas", "22222222222", "lucas@email.com",
                         "11987654322", "02/02/1991", 3000.00,
                         "senha456", 0, 0);

    // Adicionar f1
    int err = dbHashAdd(funcFile, "funcionarios", &f1, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
    printf("Adição f1 (PK 1): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");

    // Adicionar f2
    err = dbHashAdd(funcFile, "funcionarios", &f2, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
    printf("Adição f2 (PK 63): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");

    // Leitura de registros
    printf("\nBuscando registros...\n");

    TFunc r1, r2;
    r1.pk = 1;
    r2.pk = 63;

    err = dbHashFind(funcFile, "funcionarios", &r1, sizeof(TFunc), pk_offset, next_pk_offset, status_offset);
    printf("Leitura f1 (PK 1): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");
    TFunc_PrintGeneric(&r1);

    err = dbHashFind(funcFile, "funcionarios", &r2, sizeof(TFunc), pk_offset, next_pk_offset, status_offset);
    printf("Leitura f2 (PK 63): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");
    TFunc_PrintGeneric(&r2);

    // Deleção de registros
    printf("\nDeletando registros...\n");

    // Deletar f1 (PK 1)
    err = dbHashDelete(funcFile, "funcionarios", 1, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_write);
    printf("Deleção f1 (PK 1): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");

    // Buscar ambos após deleção
    printf("\nBuscando registros após deleção...\n");

    TFunc buffer1;
    TFunc buffer2;
    buffer1.pk = 1;
    buffer2.pk = 63;

    err = dbHashFind(funcFile, "funcionarios", &buffer1, sizeof(TFunc), pk_offset, next_pk_offset, status_offset);
    printf("Leitura f1 (PK 1): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");
    TFunc_PrintGeneric(&r1);

    err = dbHashFind(funcFile, "funcionarios", &buffer2, sizeof(TFunc), pk_offset, next_pk_offset, status_offset);
    printf("Leitura f2 (PK 63): %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");
    TFunc_PrintGeneric(&r2);

    printf("\nLeitura da Base...\n");

    err = dbPrintTable(funcFile, "funcionarios", pk_offset, status_offset, sizeof(TFunc),TFunc_PrintGeneric);
    printf("Leitura da Base: %d - %s\n", err, err == DB_OK ? "SUCESSO" : "FALHA");

    dbClose(funcFile);
}



#define MAX_FUNCIONARIOS 10000

int criaBase(){
    // Criar arquivo para os funcionários
    FILE *funcFile = dbInit("funcionarios.dat");
    if (funcFile == NULL) {
        printf("Erro ao inicializar o arquivo.\n");
        return -1; // Retorna erro se o arquivo não pôde ser aberto
    }

    // Criar a tabela de funcionários
    dbCreateTable(funcFile, "data/funcionarios", sizeof(TFunc));

    // Definindo os offsets
    size_t pk_offset = offsetof(TFunc, pk);
    size_t next_pk_offset = offsetof(TFunc, next_pk);
    size_t status_offset = offsetof(TFunc, status);

    // Criar base de dados desordenada
    createBase(70, 70); // Criar 100 funcionários

    // Imprimir a tabela
    dbPrintTable(funcFile, "funcionarios", pk_offset, status_offset, sizeof(TFunc), TFunc_PrintGeneric);


    TFunc f;
    f.pk = 70;

    dbHashFind(funcFile, "funcionarios", &f, sizeof(TFunc), pk_offset, next_pk_offset, status_offset);

    TFunc_PrintGeneric(&f);

   /*  TFunc b;
    fseek(funcFile, 3688, SEEK_SET);
    fread(&b, sizeof(TFunc), 1, funcFile);

    TFunc_PrintGeneric(&b);
 */
    // Fechar arquivo
    dbClose(funcFile);
}

void add_and_print_records(FILE* funcFile, size_t pk_offset, size_t next_pk_offset, size_t status_offset) {
    // Criar 10 registros manualmente
    TFunc f1 = TFunc_New(1, "Ana", "11111111111", "ana@email.com", "11987654321", "01/01/1990", 2500.00, "senha123", 0, 0);
    TFunc f2 = TFunc_New(2, "Lucas", "22222222222", "lucas@email.com", "11987654322", "02/02/1991", 3000.00, "senha456", 0, 0);
    TFunc f3 = TFunc_New(3, "Maria", "33333333333", "maria@email.com", "11987654323", "03/03/1992", 3500.00, "senha789", 0, 0);
    TFunc f4 = TFunc_New(4, "Pedro", "44444444444", "pedro@email.com", "11987654324", "04/04/1993", 4000.00, "senhaabc", 0, 0);
    TFunc f5 = TFunc_New(5, "João", "55555555555", "joao@email.com", "11987654325", "05/05/1994", 4500.00, "senhaxyz", 0, 0);
    TFunc f6 = TFunc_New(6, "Carla", "66666666666", "carla@email.com", "11987654326", "06/06/1995", 5000.00, "senhaqwe", 0, 0);
    TFunc f7 = TFunc_New(7, "Paula", "77777777777", "paula@email.com", "11987654327", "07/07/1996", 5500.00, "senhaasd", 0, 0);
    TFunc f8 = TFunc_New(8, "Carlos", "88888888888", "carlos@email.com", "11987654328", "08/08/1997", 6000.00, "senhaqaz", 0, 0);
    TFunc f9 = TFunc_New(9, "Fernanda", "99999999999", "fernanda@email.com", "11987654329", "09/09/1998", 6500.00, "senha123", 0, 0);
    TFunc f10 = TFunc_New(10, "Tiago", "00000000000", "tiago@email.com", "11987654330", "10/10/1999", 7000.00, "senha456", 0, 0);

    // Array de ponteiros para os registros
    TFunc* funcs[10] = {&f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10};

    // Adicionar os registros ao banco de dados
    for (int i = 0; i < 10; i++) {
        int err = dbHashAdd(funcFile, "funcionarios", funcs[i], sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
        if (err != 0) {
            fprintf(stderr, "Erro ao adicionar o registro %lu: %d\n", funcs[i]->pk, err);
        }
    }

    dbPrintTable(funcFile, "funcionarios", pk_offset, status_offset, sizeof(TFunc), TFunc_PrintGeneric);
}

// Função principal
int teste2() {

    run_tests2();

    //criaBase();

    /* FILE *funcFile = dbInit("funcionarios_testes.bin");
    dbCreateTable(funcFile, "funcionarios", sizeof(TFunc));

    // Definindo os offsets
    size_t pk_offset = offsetof(TFunc, pk);
    size_t next_pk_offset = offsetof(TFunc, next_pk);
    size_t status_offset = offsetof(TFunc, status);


    criaBase();
 */
    //add_and_print_records(funcFile, pk_offset, next_pk_offset, status_offset);

  /*   TFunc f1 = TFunc_New(1, "Ana", "11111111111", "ana@email.com",
                         "11987654321", "01/01/1990", 2500.00,
                         "senha123", 0, 0);

    TFunc f2 = TFunc_New(2, "Lucas", "22222222222", "lucas@email.com",
                         "11987654322", "02/02/1991", 3000.00,
                         "senha456", 0, 0);

    TFunc f3 = TFunc_New(3, "Maria", "33333333333", "maria@email.com",
                         "11987654323", "03/03/1992", 3500.00,
                         "senha789", 0, 0);

    TFunc f4 = TFunc_New(63, "Maria", "33333333333", "maria@email.com",
                         "11987654323", "03/03/1992", 3500.00,
                         "senha789", 0, 0);

    TFunc b1, b2, b3, b4;
    b1.pk = 1;
    b2.pk = 2;
    b3.pk = 3;
    b4.pk = 4;

    int err = dbHashAdd(funcFile, "funcionarios", &f1, sizeof(TFunc), pk_offset, next_pk_offset, generic_read, generic_write);
    err = dbHashAdd(funcFile, "funcionarios", &f2, sizeof(TFunc), pk_offset, next_pk_offset, generic_read, generic_write);
    err = dbHashAdd(funcFile, "funcionarios", &f3, sizeof(TFunc), pk_offset, next_pk_offset, generic_read, generic_write);
    err = dbHashAdd(funcFile, "funcionarios", &f4, sizeof(TFunc), pk_offset, next_pk_offset, generic_read, generic_write);


    err = dbHashFind(funcFile, "funcionarios", &b1, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_read, generic_write);
    TFunc_PrintGeneric(&f1);

    err = dbHashFind(funcFile, "funcionarios", &b2, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_read, generic_write);
    TFunc_PrintGeneric(&f2);

    err = dbHashFind(funcFile, "funcionarios", &b3, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_read, generic_write);
    TFunc_PrintGeneric(&f3);

    err = dbHashFind(funcFile, "funcionarios", &b4, sizeof(TFunc), pk_offset, next_pk_offset, status_offset, generic_read, generic_write);
    TFunc_PrintGeneric(&f4);



    dbPrintTable(funcFile, "funcionarios", sizeof(TFunc), pk_offset, next_pk_offset, status_offset, TFunc_PrintGeneric);
 */
    return 0;
}

