#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controler/db_wrapper.h"
#include "controler/crud.h"
#include "model/exercicio.h"
#include "model/cliente.h"
#include "model/treino.h"
#include "model/funcionario.h"
#include "utils/util.h"

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

void updateHeader(FILE *file, DatabaseHeader *header) {
    fseek(file, 0, SEEK_SET);
    fwrite(header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
}

// Função para escrever dados desordenados e atualizar a barra de progresso
void escreveClienteDesordenado(FILE *fcli, int *vet, int tam, int progress_start, int progress_end, clock_t start_time) {
    printf("\nGerando base de clientes...\n");
    DatabaseHeader header;
    fseek(fcli, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fcli);

    TCliente cliente;
    long unsigned offset = sizeof(DatabaseHeader);

    for (int i = 0; i < tam; i++) {
        cliente = TCliente_New(vet[i], "Alice", "12345678901", "alice@example.com", "(99)99999-9999", "16/04/1980");
        fwrite(&cliente, sizeof(TCliente), 1, fcli);

        // Atualiza a barra de progresso
        clock_t end_time = clock();
        double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        int progress = progress_start + (i + 1) * (progress_end - progress_start) / tam;
        atualizarBarraProgresso(progress, 100, time_spent, "Cliente:");
    }

    // Atualiza o header
    header.tables[0].end_offset = offset + (sizeof(TCliente) * tam);;
    header.tables[0].next_pk = tam + 1;
    updateHeader(fcli, &header);
}

// Funções semelhantes para Func, Exerc e Treino
void escreveFuncDesordenado(FILE *ffunc, int *vet, int tam, int progress_start, int progress_end, clock_t start_time) {
    printf("\nGerando base de funcionarios...\n");
    DatabaseHeader header;
    fseek(ffunc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ffunc);

    TFunc func;
    long unsigned offset = sizeof(DatabaseHeader);

    for (int i = 0; i < tam; i++) {
        func = TFunc_New(vet[i], "Joao", "12345678901", "joao@example.com", "(99)99999-9999", "31/12/2000", 3500.00);
        fwrite(&func, sizeof(TFunc), 1, ffunc);

        // Atualiza a barra de progresso
        clock_t end_time = clock();
        double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        int progress = progress_start + (i + 1) * (progress_end - progress_start) / tam;
        atualizarBarraProgresso(progress, 100, time_spent, "Funcionario:");
    }

    // Atualiza o header
    header.tables[0].end_offset = offset + (sizeof(TFunc) * tam);
    header.tables[0].next_pk = tam + 1;
    updateHeader(ffunc, &header);
}

void escreveExercDesordenado(FILE *fexer, int *vet, int tam, int progress_start, int progress_end, clock_t start_time) {
    printf("\nGerando base de exercicios...\n");
    DatabaseHeader header;
    fseek(fexer, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fexer);

    TExerc exerc;
    long unsigned offset = sizeof(DatabaseHeader);

    for (int i = 0; i < tam; i++) {
        exerc = TExerc_New(vet[i], "Agachamento", "Strenght", 60*15);
        fwrite(&exerc, sizeof(TExerc), 1, fexer);
      
        // Atualiza a barra de progresso
        clock_t end_time = clock();
        double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        int progress = progress_start + (i + 1) * (progress_end - progress_start) / tam;
        atualizarBarraProgresso(progress, 100, time_spent, "Exercicio:");
    }

    // Atualiza o header
    header.tables[0].end_offset = offset + (sizeof(TExerc) * tam);
    header.tables[0].next_pk = tam + 1;
    updateHeader(fexer, &header);
}

void escreveTreinoDesordenado(FILE *ftreino, int *vet, int tam, int progress_start, int progress_end, clock_t start_time) {
    printf("\nGerando base de treinos...\n");

    DatabaseHeader header;
    fseek(ftreino, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ftreino);

    TTreino treino;
    long unsigned offset = sizeof(DatabaseHeader);

    for (int i = 0; i < tam; i++) {
        treino = TTreino_New(vet[i], "Alice", "12345678901", vet[i], vet[i], 60*100);
        fwrite(&treino, sizeof(TTreino), 1, ftreino);

        // Atualiza a barra de progresso
        clock_t end_time = clock();
        double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        int progress = progress_start + (i + 1) * (progress_end - progress_start) / tam;
        atualizarBarraProgresso(progress, 100, time_spent, "Treino:");
    }

    // Atualiza o header
    header.tables[0].end_offset = offset + (sizeof(TTreino) * tam);
    header.tables[0].next_pk = tam + 1;
    updateHeader(ftreino, &header);
}

// Função principal para criar a base desordenada
int criar_base_desordenada(int tam, int qtdTrocas, FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {
    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;
    embaralha(vet, tam, qtdTrocas);

    // Tempo total para todas as operações
    clock_t start_time_global = clock();

    // Escreve em cada tabela e atualiza a barra de progresso global
    escreveClienteDesordenado(fcli, vet, tam, 0, 25, clock());
    escreveFuncDesordenado(ffunc, vet, tam, 25, 50, clock());
    escreveExercDesordenado(fexer, vet, tam, 50, 75, clock());
    escreveTreinoDesordenado(ftreino, vet, tam, 75, 100, clock());

    printf("\n"); // Adiciona uma nova linha ao final para evitar sobrescrita

    // Calcula e exibe o tempo total gasto
    clock_t end_time_global = clock();
    double total_time_spent = (double)(end_time_global - start_time_global) / CLOCKS_PER_SEC;
    printf("Tempo total gasto: %.2fs\n", total_time_spent);

    return tam;
}

int criar_base_ordenada(int tam, FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {

    int i = -1;
    clock_t start_time, end_time;
    double time_spent;

    // Inicia a contagem de tempo
    start_time = clock();

    for (i = 1; i <= tam; i++){
        cAddCliente(fcli, CLIENTES, 0, "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
        cAddFunc(ffunc, FUNCIONARIOS, 0, "Charlie", "11223344556", "charlie@example.com", "1112233445", "1985-07-30", 3500.50);
        cAddExerc(fexer, EXERCICIOS, 0, "Push-up", "Strength", 30*60);
        TCliente cliente;
        cliente.pk = i;
        TExerc exercicio;
        exercicio.pk = i;
        cAddTreinoNotC(ftreino, 0, "Morning Routine", "Strength", &exercicio, &cliente, 20*30);

        // Calcula o tempo gasto até agora
        end_time = clock();
        time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Calcula a porcentagem de progresso
        int progress = (i + 1) * 100 / tam;

        // Atualiza a barra de progresso
        atualizarBarraProgresso(progress, 100, time_spent, "Progresso:");

    }
    return i;
}

int criar_base_desordenada_2(int tam, int qtdTrocas, FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexer) {
    int vet[tam];
    for (int i = 0; i < tam; i++) vet[i] = i + 1;
    embaralha(vet, tam, qtdTrocas);

    int i = -1;
    clock_t start_time, end_time;
    double time_spent;

    // Inicia a contagem de tempo
    start_time = clock();

    for (i = 0; i < tam; i++) {
        cAddCliente(fcli, CLIENTES, vet[i], "Alice", "12345678901", "alice@example.com", "1234567890", "2024-12-31");
        cAddFunc(ffunc, FUNCIONARIOS, vet[i], "Charlie", "11223344556", "charlie@example.com", "1112233445", "1985-07-30", 3500.50);
        cAddExerc(fexer, EXERCICIOS, vet[i], "Push-up", "Strength", 30*60);
        TCliente cliente;
        cliente.pk = vet[i];
        TExerc exercicio;
        exercicio.pk = vet[i];
        cAddTreinoNotC(ftreino, vet[i], "Morning Routine", "Strength", &exercicio, &cliente, 20*60);

        // Calcula o tempo gasto até agora
        end_time = clock();
        time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        // Calcula a porcentagem de progresso
        int progress = (i + 1) * 100 / tam;

        // Atualiza a barra de progresso
        atualizarBarraProgresso(progress, 100, time_spent, "Progresso:");
    }
    
    printf("\n"); // Adiciona uma nova linha ao final do loop para que o próximo output não sobrescreva a barra

    return i;
}

int teste_criar_base_desordenada(int tam) {
    // Abrir os arquivos de banco de dados
    FILE *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    FILE *fexer = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    // Inicializar tabelas
    cInitTables(fcli, ffunc, ftreino, fexer);

    printf("Criando base de dados desordenada...\n");
    int count = criar_base_desordenada(tam, tam, fcli, ffunc, ftreino, fexer);
    if (count < 0){
        perror("Erro ao criar base de dados desordenada");
    };
    int opcao = 0;
    printf("Deseja imprimir alguma base?\n| > (0 - Não, 1 - funcionários, 2 - clientes, 3 - exercícios, 4 - treinos, 5 - todos)\n");
    scanf("%d", &opcao);
    switch (opcao) {
        case 1:
            TFunc func;
            cdbReadAll(ffunc, FUNCIONARIOS, &func, sizeof(TFunc), TFunc_PrintGeneric);
            break;
        case 2:
            TCliente cliente;
            cdbReadAll(fcli, CLIENTES, &cliente, sizeof(TCliente), TCliente_PrintGeneric);                  
            break;
        case 3:
            TExerc exercicio;
            cdbReadAll(fexer, EXERCICIOS, &exercicio, sizeof(TExerc), TExerc_PrintGeneric);
            break;
        case 4:
            TTreino treino;
            cdbReadAll(ftreino, TREINOS, &treino, sizeof(TTreino), TTreino_PrintGeneric);
            break;
        case 5:
            cdbReadAll(ffunc, FUNCIONARIOS, &func, sizeof(TFunc), TFunc_PrintGeneric);
            cdbReadAll(fcli, CLIENTES, &cliente, sizeof(TCliente), TCliente_PrintGeneric);
            cdbReadAll(fexer, EXERCICIOS, &exercicio, sizeof(TExerc), TExerc_PrintGeneric);
            cdbReadAll(ftreino, TREINOS, &treino, sizeof(TTreino), TTreino_PrintGeneric);
            break;
        default:
            break;
    }
    printf("Desalocando recursos...\n");
    count = cCloseDatabase(fcli, ffunc, ftreino, fexer);

    return count;
}

int teste_criar_base_ordenada(int tam) {
    // Abrir os arquivos de banco de dados
    FILE *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    FILE *fexer = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    // Inicializar tabelas
    cInitTables(fcli, ffunc, ftreino, fexer);

    printf("Criando base de dados ordenada...\n");
    int count = criar_base_ordenada(tam, fcli, ffunc, ftreino, fexer);
    if (count < 0){
        perror("Erro ao criar base de dados ordenada");
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

int teste_selecao_com_substituicao_func(){
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    cdbCreateTable(ffunc, FUNCIONARIOS, sizeof(TFunc));
    int ok = 0;



    printf("Teste de classificacaoInterna...\n");

    int particions = TFuncSelecaoComSubstituicao(ffunc, FUNCIONARIOS);
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
    if (ffuncOrd == NULL){
        perror("Erro ao abrir o arquivo\n");
    }
    //ok = intercalacaoBasica(ftreinoOrd, header, particions, TREINOS);
    ok = TFuncIntercalacaoComArvore(ffuncOrd, &header, particions);
    if (ok < 0) perror("Erro ao intercalar as particoes\n");

    //j = cdbReadAllNoHeader(ffuncOrd, &func, sizeof(TFunc), printFunc);
    j = cdbReadAll(ffuncOrd, FUNCIONARIOS, &func, sizeof(TFunc), TFunc_PrintGeneric);
    printf("Total de registros lidos: %d\n", j);

    printf("Numero de particoes geradas: %d\n", particions);

    cdbClose(ffunc);
    cdbClose(ffuncOrd);

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
