#include "model/funcionario.h"
#include "model/produto.h"
#include "kmldb/db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {

    const char *func_table = "funcionarios";
    const char *prod_table = "produto";

    const char* db_filename = "data/database.dat";

    FILE *file = DB_Init(db_filename);
    DB_Welcome();
    DB_CreateTable(file, func_table);
    DB_CreateTable(file, prod_table);

    DatabaseHeader* header; 
    header = DB_LoadHeader(file);
    DB_PrintHeader(header);
    
    printf("\nTestando Pesquisa por ID autogerado\n");
    
    TFunc func1 = TFunc_New(file, func_table, "Lucas", "123.456.789-00", "01/01/1990", 2500.00);
    TFunc func2 = TFunc_New(file, func_table, "João", "123.456.789-00", "01/01/1990", 7500.00);
    TProd prod1 = TProd_New(file, prod_table, "Produto A", "Descrição A", "Categoria A", "1234567890123", 10.99, 10);
    
    // Adicionar ao banco de dados
    DB_AddMember(file, func_table, &func1, sizeof(TFunc));
    DB_AddMember(file, func_table, &func2, sizeof(TFunc));
    DB_AddMember(file, prod_table, &prod1, sizeof(TProd));

    func1 = TFunc_GetByPK(file, func_table, 3);
    func2 = TFunc_GetByPK(file, func_table, 4);
    prod1 = TProd_GetByPK(file, prod_table, 1);

    TFunc_Print(func1); 
    TFunc_Print(func2);
    TProd_Print(prod1);

    header = DB_LoadHeader(file);
    DB_PrintHeader(header);

    free(header);
    fclose(file);

    return 0;
}