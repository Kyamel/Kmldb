#include "model/funcionario.h"
#include "model/produto.h"
#include "kmldb/db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *func_table = "funcionarios";
    const char *prod_table = "produto";
    const char *db_filename = "data/database.dat";

    FILE *file = DB_Init(db_filename);
    
    DB_CreateTable(file, func_table, sizeof(TFunc));
    DB_CreateTable(file, prod_table, sizeof(TProd));
    
    TProd prod1 = TProd_New("Produto A", "Descrição A", "Categoria A", "0123", 10.99, 10);
    TProd prod2 = TProd_New("Produto B", "Descrição B", "Categoria B", "5678", 5.09, 64);
    TFunc func1 = TFunc_New("Lucas", "123.456.789-00", "01/01/1990", 2500.00);
    TFunc func2 = TFunc_New("João", "123.456.789-00", "01/01/1990", 7500.00);

    // Adicionar ao banco de dados
    //DB_AddMember(file, func_table, &func1, sizeof(func1), DB_PK_OFFSET(TFunc));
    //DB_AddMember(file, func_table, &func2, sizeof(func2), DB_PK_OFFSET(TFunc));
    //DB_AddMember(file, prod_table, &prod1, sizeof(prod1), DB_PK_OFFSET(TProd));
    //DB_AddMember(file, prod_table, &prod2, sizeof(prod2), DB_PK_OFFSET(TProd));

    printf("\nTestando Pesquisa por ID autogerado\n");

    func1 = TFunc_GetByPK(file, func_table, 1);
    func2 = TFunc_GetByPK(file, func_table, 4);
    prod1 = TProd_GetByPK(file, prod_table, 1);
    prod2 = TProd_GetByPK(file, prod_table, 2);

    TFunc_Print(&func1); 
    TFunc_Print(&func2);
    TProd_Print(&prod1);
    TProd_Print(&prod2);

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        exit(EXIT_FAILURE);
    }
    DB_PrintHeader(&header);

    fseek(file, 408, SEEK_SET);
    TFunc f = TFunc_Read(file);
    printf("1");
    TFunc_Print(&f);
    
    fseek(file, 408+88, SEEK_SET);
    f = TFunc_Read(file);
    printf("2");
    TFunc_Print(&f);

    fseek(file, 408+(88*2), SEEK_SET);
    f = TFunc_Read(file);
    printf("3");
    TFunc_Print(&f);

    fseek(file, 408+(88*3), SEEK_SET);
    f = TFunc_Read(file);
    printf("4");
    TFunc_Print(&f);
    // 408 + (88*4) = 760 Onde que está o erro? Pq o prod 3 e 4 funcionam enquanto 1 e 2 não?
    fseek(file, 760, SEEK_SET);
    TProd p = TProd_Read(file);
    printf("1");
    TProd_Print(&p);

    fseek(file, 760+320, SEEK_SET);
    p = TProd_Read(file);
    printf("2");
    TProd_Print(&p);

    fseek(file, 760+(320*2), SEEK_SET);
    p = TProd_Read(file);
    printf("3");        
    TProd_Print(&p);

    fseek(file, 760+(320*3), SEEK_SET);
    p = TProd_Read(file);
    printf("4");
    TProd_Print(&p);
        
    fclose(file);
    return 0;
}
