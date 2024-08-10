#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../controler/db_wrapper.h"
#include "../controler/crud.h"
#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

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
    cAddTreinoNotC(ftreino, TREINOS, 0, "Morning Routine", "Strength", &exercicio, &cliente);  // Cliente 1, Exercício 1
    cAddTreinoNotC(ftreino, TREINOS, 0, "Evening Routine", "Strength", &exercicio2, &cliente2);  // Cliente 2, Exercício 2

    // Buscar e imprimir treinos
    printf("\nTreinos:\n");
    TTreino treino = cSearchTreinoComp(ftreino, TREINOS, 1, 1);  // Exercício 1, Cliente 1
    cPrintTreino(&treino);
    treino = cSearchTreinoComp(ftreino, TREINOS, 2, 2);  // Exercício 2, Cliente 2
    cPrintTreino(&treino);

    // Fechar os arquivos
    printf("Desalocando recursos...\n");
    cCloseDatabase(fcli, ffunc, ftreino, fexer);

    return 0;
}

// Função de callback para imprimir MyRecord
void printTreino(void* member) {
    TTreino* treino = (TTreino*)member;

    printf("# Treino:\n");
    printf("| pk: %lu\n", treino->pk);
    printf("| Cliente cpk: %lu\n", treino->cpk);
    printf("| Exercicio epk: %lu\n", treino->epk);
    printf("| Nome: %s\n", treino->nome);
    printf("| Tipo: %s\n", treino->tipo);
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
        cAddTreinoNotC(ftreino, TREINOS, i, "Morning Routine", "Strength", &exercicio, &cliente);

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
        cAddTreinoNotC(ftreino, TREINOS, vet[i], "Morning Routine", "Strength", &exercicio, &cliente);

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
    int count = criar_base_desordenada(100, 100, fcli, ffunc, ftreino, fexer);
    if (count < 0){
        perror("Erro ao criar base de dados desordenada");
    };

    // Buscar e imprimir TREINOS
    TTreino treino;
    cdbReadAll(ftreino, TREINOS, &treino, sizeof(TTreino), printTreino);
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
    int particions = selecaoComSubstituicao(ftreino, TREINOS);
    printf("Particoes: %d\n", particions);

    int j = 0;
    TTreino treino = {0};
    for (int i = 0; i < particions; i++){
        printf("\nImprimindo Particao %d:\n", i);
        char nome_arq[256];
        sprintf_s(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", i);

        FILE *particionFile = fopen(nome_arq, "r+b");
        j += cdbReadAllNoHeader(particionFile, &treino, sizeof(TTreino), printTreino);
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