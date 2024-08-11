#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../controler/db_wrapper.h"
#include "../controler/crud.h"
#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

#include "../utils/util.h"

#include <stdlib.h>
#include <time.h>
#include "teste.h"

void embaralha(int *vet, int tam, int qtdTrocas) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < qtdTrocas; i++) {
        int a = rand() % tam;
        int b = rand() % tam;
        int temp = vet[a];
        vet[a] = vet[b];
        vet[b] = temp;
    }
}

// Função para executar o teste
int run_test() {
    // Abrir os arquivos de banco de dados
    FILE *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    FILE *fexer = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    // Inicializar tabelas
    cInitTables(fcli, ffunc, ftreino, fexer);

    // Inserir dados
    printf("Inserindo dados...\n");

    // Cadastro de clientes
    cAddCliente(fcli, CLIENTES, 0, "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
    cAddCliente(fcli, CLIENTES, 0, "Bob", "98765432100", "bob@example.com", "0987654321", "2025-12-31");

    // Cadastro de funcionários
    cAddFunc(ffunc, FUNCIONARIOS, 0, "Charlie", "11223344556", "charlie@example.com", "1112233445", "1985-07-30", 3500.50);
    cAddFunc(ffunc, FUNCIONARIOS, 0, "Dana", "66554433221", "dana@example.com", "5544332211", "1990-11-15", 4200.75);

    // Cadastro de exercícios
    cAddExerc(fexer, EXERCICIOS, 0, "Push-up", "Strength", 60);
    cAddExerc(fexer, EXERCICIOS, 0, "Squat", "Strength", 90);

    // Buscar e imprimir dados
    printf("\nBuscando e imprimindo dados...\n");

    // Buscar e imprimir funcionários
    printf("\nFuncionários:\n");
    TFunc funcionario = TFunc_GetByPK(ffunc, FUNCIONARIOS, 1);  // ID 1 para Charlie
    cPrintFunc(&funcionario);
    funcionario = TFunc_GetByPK(ffunc, FUNCIONARIOS, 2);  // ID 2 para Dana
    cPrintFunc(&funcionario);

    // Buscar e imprimir clientes
    printf("\nClientes:\n");
    TCliente cliente = TCliente_GetByPK(fcli, CLIENTES, 1);  // ID 1 para Alice
    cPrintCliente(&cliente);
    TCliente cliente2 = TCliente_GetByPK(fcli, CLIENTES, 2);  // ID 2 para Bob
    cPrintCliente(&cliente);

    // Buscar e imprimir exercícios
    printf("\nExercícios:\n");
    TExerc exercicio = cSearchExerc(fexer, EXERCICIOS, 1);  // ID 1 para Push-up
    cPrintExerc(&exercicio);
    TExerc exercicio2 = cSearchExerc(fexer, EXERCICIOS, 2);  // ID 2 para Squat
    cPrintExerc(&exercicio);

    // Cadastro de treinos
    cAddTreinoNotC(ftreino, 0, "Morning Routine", "Strength", &exercicio, &cliente, 20*30);  // Cliente 1, Exercício 1
    cAddTreinoNotC(ftreino, 0, "Evening Routine", "Strength", &exercicio2, &cliente2, 20*30);  // Cliente 2, Exercício 2

    // Buscar e imprimir treinos
    printf("\nTreinos:\n");
    TTreino treino = cSearchTreinoByCpkEpk(ftreino, 1, 1);  // Exercício 1, Cliente 1
    cPrintTreino(&treino);
    treino = cSearchTreinoByCpkEpk(ftreino, 2, 2);  // Exercício 2, Cliente 2
    cPrintTreino(&treino);

    // Fechar os arquivos
    printf("Desalocando recursos...\n");
    cCloseDatabase(fcli, ffunc, ftreino, fexer);

    return 0;
}

int criar_base_ordenada(int tam, FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {

    int i = -1;
    for (i = 1; i <= tam; i++){
        cAddCliente(fcli, CLIENTES, i, "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
        cAddFunc(ffunc, FUNCIONARIOS, i, "Charlie", "11223344556", "charlie@example.com", "1112233445", "1985-07-30", 3500.50);
        cAddExerc(fexer, EXERCICIOS, i, "Push-up", "Strength", 30*60);
        TCliente cliente;
        cliente.pk = i;
        TExerc exercicio;
        exercicio.pk = i;
        cAddTreinoNotC(ftreino, i, "Morning Routine", "Strength", &exercicio, &cliente, 20*30);

    }
    return i;
}

int criar_base_desordenada(int tam, int qtdTrocas, FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {
    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;
    embaralha(vet, tam, qtdTrocas);

    int i = -1;
    for (i = 0; i < tam; i++){
        cAddCliente(fcli, CLIENTES, vet[i], "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
        cAddFunc(ffunc, FUNCIONARIOS, vet[i], "Charlie", "11223344556", "charlie@example.com", "1112233445", "1985-07-30", 3500.50);
        cAddExerc(fexer, EXERCICIOS, vet[i], "Push-up", "Strength", 30*60);
        TCliente cliente;
        cliente.pk = vet[i];
        TExerc exercicio;
        exercicio.pk = vet[i];
        cAddTreinoNotC(ftreino, vet[i], "Morning Routine", "Strength", &exercicio, &cliente, 20*60);

    }
    return i;
}

int teste_criar_base_desordenada() {
    // Abrir os arquivos de banco de dados
    FILE *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    FILE *fexer = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    // Inicializar tabelas
    cInitTables(fcli, ffunc, ftreino, fexer);

    printf("Criando base de dados desordenada...\n");
    int count = criar_base_desordenada(1000, 1000, fcli, ffunc, ftreino, fexer);
    if (count < 0){
        perror("Erro ao criar base de dados desordenada");
    };

    // Buscar e imprimir TREINOS
    TTreino treino;
    cdbReadAll(ftreino, TREINOS, &treino, sizeof(TTreino), TTreino_PrintGeneric);
    printf("Quantidade de treinos: %d\n", count);

    printf("Desalocando recursos...\n");
    count = cCloseDatabase(fcli, ffunc, ftreino, fexer);

    return count;
}

int particion_test() {
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    cdbCreateTable(ftreino, TREINOS, sizeof(TTreino));
    int ok = 0;

    printf("Teste de selecao com substituiao aplicado ao cpk do treino...\n");
    int particions = TTreinoSelecaoComSubstituicaoCpkk(ftreino, TREINOS);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TTreino treino = {0};
    for (int i = 0; i < particions; i++){
        printf("\nImprimindo Particao %d:\n", i);
        char nome_arq[256];
        sprintf_s(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", i);

        FILE *particionFile = fopen(nome_arq, "r+b");
        j += cdbReadAllNoHeader(particionFile, &treino, sizeof(TTreino), TTreino_PrintGeneric);
        fclose(particionFile);
    }
    printf("Particoes geradas: %d\n", particions);
    printf("Total de registros lidos: %d\n", j);

   /*  // Escreve o cabeçalho no início do arquivo de saída
    FILE *ftreinoOrd =  fopen(DB_FOLDER"/"TREINOS"COrdCpk.dat", "w+b");

    ok = intercalacaoBasica(ftreinoOrd, particions);

    TTreino treino;
    int j = cdbReadAllNoHeader(ftreinoOrd, &treino, sizeof(TTreino), printTreino);
    printf("Total de registros lidos: %d\n", j);

    cdbClose(ftreinoOrd); */
    cdbClose(ftreino);
    return ok;
}

int teste_classificasao_interna_func(){
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    cdbCreateTable(ffunc, FUNCIONARIOS, sizeof(TFunc));
    int ok = 0;

    printf("Teste de classificacaoInterna...\n");

    int particions = TFuncClassificacaoInterna(ffunc, FUNCIONARIOS);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TFunc func;
    /* for (int i = 0; i < particions; i++){
        printf("\nImprimindo Particao %d:\n", i);
        char nome_arq[256];
        sprintf_s(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", i);

        FILE *particionFile = fopen(nome_arq, "r+b");
        j += cdbReadAllNoHeader(particionFile, &func, sizeof(TFunc), printFunc);
        fclose(particionFile);
    }
    printf("Particoes geradas: %d\n", particions);
    printf("Total de registros lidos: %d\n", j); */    

    // Carrega o cabeçalho
    DatabaseHeader header;
    fseek(ffunc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ffunc);

    FILE *ffuncOrd =  fopen(DB_FOLDER"/"FUNCIONARIOS"COrd.dat", "w+b");
    //ok = intercalacaoBasica(ftreinoOrd, header, particions, TREINOS);
    ok = TFuncIntercalacaoBasica(ffuncOrd, &header, particions);
    if (ok < 0) perror("Erro ao intercalar as particoes\n");

    //j = cdbReadAllNoHeader(ffuncOrd, &func, sizeof(TFunc), printFunc);
    j = cdbReadAll(ffuncOrd, FUNCIONARIOS, &func, sizeof(TFunc), TFunc_PrintGeneric);
    printf("Total de registros lidos: %d\n", j);

    cdbClose(ffunc);
    //  cdbClose(ffuncOrd); 

    return 0;
}

int teste_classificasao_interna_treino(){
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    cdbCreateTable(ftreino, TREINOS, sizeof(TTreino));
    int ok = 0;

    printf("Teste de classificacaoInterna...\n");

    int particions = TTreinoClassificacaoInterna(ftreino, TREINOS);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TTreino treino;
    /* for (int i = 0; i < particions; i++){
        printf("\nImprimindo Particao %d:\n", i);
        char nome_arq[256];
        sprintf_s(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", i);

        FILE *particionFile = fopen(nome_arq, "r+b");
        j += cdbReadAllNoHeader(particionFile, &func, sizeof(TFunc), printFunc);
        fclose(particionFile);
    }
    printf("Particoes geradas: %d\n", particions);
    printf("Total de registros lidos: %d\n", j); */    

    // Carrega o cabeçalho
    DatabaseHeader header;
    fseek(ftreino, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ftreino);

    FILE *ftreinoOrd =  fopen(DB_FOLDER"/"TREINOS"COrd.dat", "w+b");
    //ok = intercalacaoBasica(ftreinoOrd, header, particions, TREINOS);
    ok = TTreinoIntercalacaoBasica(ftreinoOrd, &header, particions);
    if (ok < 0) perror("Erro ao intercalar as particoes\n");

    //j = cdbReadAllNoHeader(ftreinoOrd, &treino, sizeof(TTreino), printTreino);
    j = cdbReadAll(ftreinoOrd, TREINOS, &treino, sizeof(TTreino), TTreino_PrintGeneric);
    printf("Total de registros lidos: %d\n", j);

    cdbClose(ftreino);
    //  cdbClose(ffuncOrd); 

    return 0;
}

int teste_classificasao_interna_exerc() {
    FILE *fexerc = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");
    cdbCreateTable(fexerc, EXERCICIOS, sizeof(TExerc));
    int ok = 0;

    printf("Teste de classificacaoInterna para TExerc...\n");

    int particions = TExercClassificacaoInterna(fexerc, EXERCICIOS);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TExerc exerc;

    // Carrega o cabeçalho
    DatabaseHeader header;
    fseek(fexerc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fexerc);

    FILE *fexercOrd =  fopen(DB_FOLDER"/"EXERCICIOS"COrd.dat", "w+b");
    ok = TExercIntercalacaoBasica(fexercOrd, &header, particions);
    if (ok < 0) perror("Erro ao intercalar as particoes\n");

    j = cdbReadAll(fexercOrd, EXERCICIOS, &exerc, sizeof(TExerc), TExerc_PrintGeneric);
    printf("Total de registros lidos: %d\n", j);

    cdbClose(fexerc);
    cdbClose(fexercOrd);

    return 0;
}

int teste_classificasao_interna_cliente() {
    FILE *fcliente = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    cdbCreateTable(fcliente, CLIENTES, sizeof(TCliente));
    int ok = 0;

    printf("Teste de classificacaoInterna para TCliente...\n");

    int particions = TClienteClassificacaoInterna(fcliente, CLIENTES);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TCliente cliente;

    // Carrega o cabeçalho
    DatabaseHeader header;
    fseek(fcliente, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fcliente);

    FILE *fclienteOrd =  fopen(DB_FOLDER"/"CLIENTES"COrd.dat", "w+b");
    ok = TClienteIntercalacaoBasica(fclienteOrd, &header, particions);
    if (ok < 0) perror("Erro ao intercalar as particoes\n");

    j = cdbReadAll(fclienteOrd, CLIENTES, &cliente, sizeof(TCliente), TCliente_PrintGeneric);
    printf("Total de registros lidos: %d\n", j);

    cdbClose(fcliente);
    cdbClose(fclienteOrd);

    return 0;
}
