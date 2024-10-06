#include <time.h>

#include "../kmldb/db.h"

#include "../model/funcionario.h"
#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"

#include "../utils/util.h"


// Função para embaralhar um vetor de inteiros
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

#define MAX_FUNCIONARIOS 10000
#define MAX_EXERCICIOS 10000
#define MAX_CLIENTES 10000
#define MAX_TREINOS 10000

void criaBaseFuncionarios(FILE* file, int num_funcionarios, int num_trocas) {
    if (num_funcionarios > MAX_FUNCIONARIOS) {
        printf("O número máximo de funcionários é %d.\n", MAX_FUNCIONARIOS);
        return;
    }

    size_t pk_offset = offsetof(TFunc, pk);
    size_t next_pk_offset = offsetof(TFunc, next_pk);
    size_t status_offset = offsetof(TFunc, status);

    // Gerar chaves primárias em ordem
    int pks[MAX_FUNCIONARIOS];
    for (int i = 0; i < num_funcionarios; i++) {
        pks[i] = i; // Inicializa com PKs em ordem
    }

    // Embaralhar as chaves primárias
    embaralha(pks, num_funcionarios, num_trocas); // Embaralhamento

    // Adicionar funcionários à base de dados
    TFunc funcionario = {0};
    char func_name[BF_NOME];
    clock_t start, end;

    for (int i = 0; i < num_funcionarios; i++) {

        start = clock();

        // Cria um nome variando com i
        sprintf_s(func_name, sizeof(func_name), "Funcionario %d", i + 1); // Ex: "Funcionario 1", "Funcionario 2", etc.

        funcionario = TFunc_New(
            pks[i] + 1,
            func_name,      // Nome variado
            "12345678901",        // CPF padrão
            "funcionario@email.com", // Email padrão
            "11900000000",        // Telefone padrão
            "01/01/1990",         // Data de nascimento padrão
            2500.00,              // Salário padrão
            "senha123",           // Senha padrão
            0,                    // Status: 0 - ativo
            0                     // next_pk
        );

        int err = dbHashAdd(file, "funcionarios", &funcionario, sizeof(TFunc), pk_offset, next_pk_offset, generic_write);
        //printf("Adição de Funcionario (PK %lu): cod %d - %s\n", funcionario.pk, err, err == DB_OK ? "SUCESSO" : "FALHA");

        end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        atualizarBarraProgresso(i, num_funcionarios, time_spent, "Adicionando clientes...");
    }
}

void createExercBase(FILE* file, int num_exercicios, int num_trocas) {
    if (num_exercicios > MAX_EXERCICIOS) {
        printf("O número máximo de exercícios é %d.\n", MAX_EXERCICIOS);
        return;
    }

    size_t pk_offset = offsetof(TExerc, pk);
    size_t next_pk_offset = offsetof(TExerc, next_pk);
    size_t status_offset = offsetof(TExerc, status);

    int pks[MAX_EXERCICIOS];
    for (int i = 0; i < num_exercicios; i++) {
        pks[i] = i;
    }

    embaralha(pks, num_exercicios, num_trocas);

    TExerc exercicio = {0};
    char exerc_name[BF_NOME];
    clock_t start, end;

    for (int i = 0; i < num_exercicios; i++) {
        start = clock();

        sprintf_s(exerc_name, sizeof(exerc_name), "Exercicio %d", i + 1); // Nome do exercício

        exercicio = TExerc_New(
            pks[i] + 1,
            exerc_name,    // Nome variado
            "Tipo Exemplo", // Tipo de exercício
            60,            // Duração padrão (60 min)
            0,             // Próxima PK
            0              // Status (ativo)
        );

        int err = dbHashAdd(file, "exercicios", &exercicio, sizeof(TExerc), pk_offset, next_pk_offset, generic_write);
        //printf("Adição de Exercicio (PK %lu): cod %d - %s\n", exercicio.pk, err, err == DB_OK ? "SUCESSO" : "FALHA");

        end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        atualizarBarraProgresso(i, num_exercicios, time_spent, "Adicionando clientes...");
    }
}

void createTreinoBase(FILE* file, int num_treinos, int num_trocas) {
    if (num_treinos > MAX_TREINOS) {
        printf("O número máximo de treinos é %d.\n", MAX_TREINOS);
        return;
    }

    size_t pk_offset = offsetof(TTreino, pk);
    size_t next_pk_offset = offsetof(TTreino, next_pk);
    size_t status_offset = offsetof(TTreino, status);

    int pks[MAX_TREINOS];
    for (int i = 0; i < num_treinos; i++) {
        pks[i] = i;
    }

    embaralha(pks, num_treinos, num_trocas);

    TTreino treino = {0};
    char treino_name[BF_NOME];
    clock_t start, end;

    for (int i = 0; i < num_treinos; i++) {
        start = clock();

        sprintf_s(treino_name, sizeof(treino_name), "Treino %d", i + 1); // Nome do treino

        treino = TTreino_New(
            pks[i] + 1,
            treino_name,     // Nome variado
            "Tipo Treino",   // Tipo de treino
            pks[i] + 1,               // PK de cliente exemplo
            pks[i] + 1,               // PK de exercício exemplo
            90,              // Duração padrão (90 min)
            0,               // Próxima PK
            0                // Status (ativo)
        );

        int err = dbHashAdd(file, "treinos", &treino, sizeof(TTreino), pk_offset, next_pk_offset, generic_write);
        //printf("Adição de Treino (PK %lu): cod %d - %s\n", treino.pk, err, err == DB_OK ? "SUCESSO" : "FALHA");

        end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        atualizarBarraProgresso(i, num_treinos, time_spent, "Adicionando clientes...");
    }
}

void createClienteBase(FILE* file, int num_clientes, int num_trocas) {
    if (num_clientes > MAX_CLIENTES) {
        printf("O número máximo de clientes é %d.\n", MAX_CLIENTES);
        return;
    }

    size_t pk_offset = offsetof(TCliente, pk);
    size_t next_pk_offset = offsetof(TCliente, next_pk);
    size_t status_offset = offsetof(TCliente, status);

    int pks[MAX_CLIENTES];
    for (int i = 0; i < num_clientes; i++) {
        pks[i] = i;
    }

    embaralha(pks, num_clientes, num_trocas);

    TCliente cliente = {0};
    char cliente_name[BF_NOME];
    clock_t start, end;

    for (int i = 0; i < num_clientes; i++) {
        start = clock();

        sprintf_s(cliente_name, sizeof(cliente_name), "Cliente %d", i + 1); // Nome do cliente

        cliente = TCliente_New(
            pks[i] + 1,
            cliente_name,     // Nome variado
            "12345678901",    // CPF padrão
            "cliente@email.com", // Email padrão
            "11900000000",    // Telefone padrão
            "01/01/1990",     // Data de nascimento padrão
            0,                // Próxima PK
            0                 // Status (ativo)
        );

        int err = dbHashAdd(file, "clientes", &cliente, sizeof(TCliente), pk_offset, next_pk_offset, generic_write);
        //printf("Adição de Cliente (PK %lu): cod %d - %s\n", cliente.pk, err, err == DB_OK ? "SUCESSO" : "FALHA");

        end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        atualizarBarraProgresso(i, num_clientes, time_spent, "Adicionando clientes...");
    }
}


void createBase(int base_size, int num_trocas) {
    // Inicializar arquivos
    FILE *funcFile = dbInit("data/funcionarios.dat");
    FILE *exercFile = dbInit("data/exercicios.dat");
    FILE *clienteFile = dbInit("data/clientes.dat");
    FILE *treinoFile = dbInit("data/treinos.dat");

    if (funcFile == NULL || exercFile == NULL || clienteFile == NULL || treinoFile == NULL) {
        printf("Erro ao inicializar os arquivos.\n");
        return;
    }

    // Criar as tabelas
    dbCreateTable(funcFile, "funcionarios", sizeof(TFunc));
    dbCreateTable(exercFile, "exercicios", sizeof(TExerc));
    dbCreateTable(clienteFile, "clientes", sizeof(TCliente));
    dbCreateTable(treinoFile, "treinos", sizeof(TTreino));

    // Definir offsets para impressão
    size_t func_pk_offset = offsetof(TFunc, pk);
    size_t func_next_pk_offset = offsetof(TFunc, next_pk);
    size_t func_status_offset = offsetof(TFunc, status);

    size_t exerc_pk_offset = offsetof(TExerc, pk);
    size_t exerc_next_pk_offset = offsetof(TExerc, next_pk);
    size_t exerc_status_offset = offsetof(TExerc, status);

    size_t cliente_pk_offset = offsetof(TCliente, pk);
    size_t cliente_next_pk_offset = offsetof(TCliente, next_pk);
    size_t cliente_status_offset = offsetof(TCliente, status);

    size_t treino_pk_offset = offsetof(TTreino, pk);
    size_t treino_next_pk_offset = offsetof(TTreino, next_pk);
    size_t treino_status_offset = offsetof(TTreino, status);

    // Criar as bases de dados
    criaBaseFuncionarios(funcFile, base_size, num_trocas); // Criar funcionários
    createExercBase(exercFile, base_size, num_trocas); // Criar exercícios
    createClienteBase(clienteFile, base_size, num_trocas); // Criar clientes
    createTreinoBase(treinoFile, base_size, num_trocas); // Criar treinos

    // Opções de impressão
    int opcao;
    printf("\nEscolha a base de dados que deseja imprimir:\n");
    printf("1 - Imprimir base de Funcionários\n");
    printf("2 - Imprimir base de Exercícios\n");
    printf("3 - Imprimir base de Clientes\n");
    printf("4 - Imprimir base de Treinos\n");
    printf("5 - Imprimir todas as bases\n");
    printf("0 - Sair\n");
    scanf_s("%d", &opcao);

    // Imprimir conforme escolha do usuário
    switch (opcao) {
        case 1:
            dbPrintTable(funcFile, "funcionarios", func_pk_offset, func_status_offset, sizeof(TFunc), TFunc_PrintGeneric);
            break;
        case 2:
            dbPrintTable(exercFile, "exercicios", exerc_pk_offset, exerc_status_offset, sizeof(TExerc), TExerc_PrintGeneric);
            break;
        case 3:
            dbPrintTable(clienteFile, "clientes", cliente_pk_offset, cliente_status_offset, sizeof(TCliente), TCliente_PrintGeneric);
            break;
        case 4:
            dbPrintTable(treinoFile, "treinos", treino_pk_offset, treino_status_offset, sizeof(TTreino), TTreino_PrintGeneric);
            break;
        case 5:
            printf("\nImprimindo todas as bases:\n");
            dbPrintTable(funcFile, "funcionarios", func_pk_offset, func_status_offset, sizeof(TFunc), TFunc_PrintGeneric);
            dbPrintTable(exercFile, "exercicios", exerc_pk_offset, exerc_status_offset, sizeof(TExerc), TExerc_PrintGeneric);
            dbPrintTable(clienteFile, "clientes", cliente_pk_offset, cliente_status_offset, sizeof(TCliente), TCliente_PrintGeneric);
            dbPrintTable(treinoFile, "treinos", treino_pk_offset, treino_status_offset, sizeof(TTreino), TTreino_PrintGeneric);
            break;
        case 0:
            printf("Nenhuma base será impressa.\n");
            break;
        default:
            printf("Opção inválida.\n");
            break;
    }

    // Fechar arquivos
    dbClose(funcFile);
    dbClose(exercFile);
    dbClose(clienteFile);
    dbClose(treinoFile);
}
