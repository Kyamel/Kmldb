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
void run_test() {
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
    cCloseDatabase(fcli, ffunc, ftreino, fexer);
}

/* int criar_base_desordenada(int tam, int qtdTrocas){

    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;

    embaralha(vet, tam, qtdTrocas);

    cAddCliente(fcli, CLIENTES, 0, "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
    cAddCliente(fcli, CLIENTES, 0, "Bob", "98765432100", "bob@example.com", "0987654321", "2025-12-31");

} */