#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../controler/db_wrapper.h"
#include "../../controler/crud.h"

#include "../../model/exercicio.h"
#include "../../model/cliente.h"
#include "../../model/treino.h"
#include "../../model/funcionario.h"

#include "../../utils/util.h"

#include "main_menu.h"
#include "../../teste.h"

void cadastrarCliente(FILE *file) {
    char nome[BC_NOME], cpf[BC_CPF], email[BC_EMAIL], telefone[BC_TELEFONE], exp_date[BC_EXP_DATE];

    printf("Cadastro de Cliente:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;  // Remove a nova linha

    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    printf("Email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    printf("Telefone: ");
    fgets(telefone, sizeof(telefone), stdin);
    telefone[strcspn(telefone, "\n")] = 0;

    printf("Data de Expiracao (DD-MM-AAAA): ");
    fgets(exp_date, sizeof(exp_date), stdin);
    exp_date[strcspn(exp_date, "\n")] = 0;

    cAddCliente(file, CLIENTES, 0, nome, cpf, email, telefone, exp_date);
    printf("Cliente cadastrado com sucesso!\n");
}

void buscarCliente(FILE *file) {
    printf("Buscar Cliente:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TCliente cliente = cSearchCliente(file, CLIENTES, pk);
    if (cliente.pk != 0) {
        TCliente_PrintGeneric(&cliente);
    } else {
        printf("Cliente nao encontrado.\n");
    }
}

void cadastrarFuncionario(FILE *file) {
    char nome[BF_NOME], cpf[BF_CPF], email[BF_EMAIL], telefone[BF_TELEFONE], data_nascimento[BF_DATA_NASCIMENTO];
    double salario;

    printf("Cadastro de Funcionario:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    printf("Email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = 0;

    printf("Telefone: ");
    fgets(telefone, sizeof(telefone), stdin);
    telefone[strcspn(telefone, "\n")] = 0;

    printf("Data de Nascimento (AAAA-MM-DD): ");
    fgets(data_nascimento, sizeof(data_nascimento), stdin);
    data_nascimento[strcspn(data_nascimento, "\n")] = 0;

    printf("Salário: ");
    scanf_s("%lf", &salario);
    cClearInputBuffer();

    cAddFunc(file, FUNCIONARIOS, 0, nome, cpf, email, telefone, data_nascimento, salario);
    printf("Funcionário cadastrado com sucesso!\n");
}

void buscarFuncionario(FILE *file) {
    printf("Buscar Funcionário:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TFunc func = TFunc_GetByPK(file, FUNCIONARIOS, pk);
    if (func.pk != 0) {
        cPrintFunc(&func);
    } else {
        printf("Funcionario nao encontrado.\n");
    }
}

void cadastrarExercicio(FILE *file) {
    char nome[BE_NOME], tipo[BE_TIPO];
    int duration;

    printf("Cadastro de Exercicio:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    char durationStr[20];
    printf("Duracao (segundos): ");
    fgets(durationStr, sizeof(durationStr), stdin);
    durationStr[strcspn(durationStr, "\n")] = 0;

    // Avalia a expressão ou converte o número diretamente
    duration = evaluateArithmeticExpression(durationStr);

    cAddExerc(file, EXERCICIOS, 0, nome, tipo, duration);
    printf("Exercicio cadastrado com sucesso!\n");
}

void buscarExercicio(FILE *file) {
    printf("Buscar Exercicio:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TExerc exercicio = cSearchExerc(file, EXERCICIOS, pk);
    if (exercicio.pk != 0) {
        cPrintExerc(&exercicio);
    } else {
        printf("Exercicio nao encontrado.\n");
    }
}

void cadastrarTreino(FILE *ftreino, FILE *fexerc, FILE *fcli) {
    char nome[BT_NOME], tipo[BT_TIPO];
    unsigned long cpk, epk;

    printf("Cadastro de Treino:\n");

    printf("PK do Cliente: ");
    scanf_s("%lu", &cpk);
    cClearInputBuffer();

    printf("PK do Exercicio: ");
    scanf_s("%lu", &epk);
    cClearInputBuffer();

    TExerc exercicio = cSearchExerc(fexerc, EXERCICIOS, epk);
    if (exercicio.pk <= 0) {
        printf("Exercicio nao encontrado.\n");
        return;
    }
    TExerc_Print(&exercicio);

    TCliente cliente = cSearchCliente(fcli, CLIENTES, cpk);
    if (cliente.pk <= 0) {
        printf("Cliente nao encontrado.\n");
        return;
    }
    TCliente_Print(&cliente);

    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    char durationStr[20];
    int duration;

    printf("Duracao do Exercicio (segundos): ");
    fgets(durationStr, sizeof(durationStr), stdin);
    durationStr[strcspn(durationStr, "\n")] = 0;

    // Avalia a expressão ou converte o número diretamente
    duration = evaluateArithmeticExpression(durationStr);

    cAddTreinoNotC(ftreino, 0, nome, tipo, &exercicio, &cliente, duration);
    printf("Treino cadastrado com sucesso!\n");
}

void buscarTreino(FILE *ftreino, FILE *fexerc) {
    printf("Buscar Treino:\n");
    unsigned long cpk;
   
    printf("Cliente PK: ");
    scanf_s("%lu", &cpk);
    cClearInputBuffer();

    int ok = cSearchPrintTreinoFullByCpk(ftreino, fexerc, cpk);
    if (ok < 0) {
        printf("Treino nao encontrado.\n");
    }
}

// Função para ordenar e substituir arquivos
void ordenarESubstituirArquivos(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexerc) {
    // Variáveis auxiliares para armazenar o número de partições e o cabeçalho do banco de dados
    int particions;
    DatabaseHeader header;
    int totalArquivos = 4; // Total de tipos de arquivos para ordenação
    int progressoAtual = 0;

    // Inicia a contagem de tempo
    clock_t start_time = clock();
    clock_t start_global_time = clock();

    // FUNCIONARIOS
    printf("\nOrdenando funcionarios...\n");
    particions = TFuncClassificacaoInterna(ffunc, FUNCIONARIOS);
    fseek(ffunc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ffunc);
    FILE *ffuncOrd = fopen(DB_FOLDER"/"FUNCIONARIOS".dat", "w+b");
    TFuncIntercalacaoBasica(ffuncOrd, &header, particions);
    fclose(ffuncOrd);
    fclose(ffunc);
    progressoAtual++;
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Funcionarios");

    // CLIENTES
    printf("\nOrdenando clientes...\n");
    start_time = clock();
    particions = TClienteClassificacaoInterna(fcli, CLIENTES);
    fseek(fcli, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fcli);
    FILE *fcliOrd = fopen(DB_FOLDER"/"CLIENTES".dat", "w+b");
    TClienteIntercalacaoBasica(fcliOrd, &header, particions);
    fclose(fcliOrd);
    fclose(fcli);
    progressoAtual++;
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Clientes");

    // EXERCICIOS
    printf("\nOrdenando exercicios...\n");
    start_time = clock();
    particions = TExercClassificacaoInterna(fexerc, EXERCICIOS);
    fseek(fexerc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fexerc);
    FILE *fexercOrd = fopen(DB_FOLDER"/"EXERCICIOS".dat", "w+b");
    TExercIntercalacaoBasica(fexercOrd, &header, particions);
    fclose(fexercOrd);
    fclose(fexerc);
    progressoAtual++;
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Exercicios");

    // TREINOS
    printf("\nOrdenando treinos...\n");
    start_time = clock();
    particions = TTreinoClassificacaoInterna(ftreino, TREINOS);
    fseek(ftreino, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ftreino);
    FILE *ftreinoOrd = fopen(DB_FOLDER"/"TREINOS".dat", "w+b");
    TTreinoIntercalacaoBasica(ftreinoOrd, &header, particions);
    fclose(ftreinoOrd);
    fclose(ftreino);
    progressoAtual++;
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Treinos");

    clock_t end_global_time = clock();
    time_spent = (double)(end_global_time - start_global_time) / CLOCKS_PER_SEC;
    printf("\nProcesso concluido em %.2f segundos.\n", time_spent);
}

void trocarParaArquivosOrdenados(FILE **fcli, FILE **ffunc, FILE **ftreino, FILE **fexerc) {
    // Fechando arquivos não ordenados
    fclose(*fcli);
    fclose(*ffunc);
    fclose(*ftreino);
    fclose(*fexerc);

    // Reabrindo arquivos ordenados
    *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    *fexerc = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    // Recarregar as tabelas se necessário
    cInitTables(*fcli, *ffunc, *ftreino, *fexerc);
}

int cli_main_menu() {

    FILE *fcli;
    FILE *ffunc;
    FILE *ftreino;
    FILE *fexerc;

    // Inicialmente, abre os arquivos não ordenados
    fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    fexerc = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    cInitTables(fcli, ffunc, ftreino, fexerc);
    cdbWelcome();

    int opcao;

    while (1) {
        printf("\nMenu Principal:\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Buscar Cliente\n");
        printf("3. Cadastrar Funcionario\n");
        printf("4. Buscar Funcionario\n");
        printf("5. Cadastrar Exercicio\n");
        printf("6. Buscar Exercicio\n");
        printf("7. Cadastrar Treino\n");
        printf("8. Buscar Treino\n");
        printf("9. Sair\n");
        printf("10. Ordenar e Substituir Arquivos\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1:
                cadastrarCliente(fcli);
                break;
            case 2:
                buscarCliente(fcli);
                break;
            case 3:
                cadastrarFuncionario(ffunc);
                break;
            case 4:
                buscarFuncionario(ffunc);
                break;
            case 5:
                cadastrarExercicio(fexerc);
                break;
            case 6:
                buscarExercicio(fexerc);
                break;
            case 7:
                cadastrarTreino(ftreino, fexerc, fcli);
                break;
            case 8:
                buscarTreino(ftreino, fexerc);
                break;
            case 9: {
                int ok = cCloseDatabase(fcli, ffunc, ftreino, fexerc);
                return ok;
            }
            case 10:
                ordenarESubstituirArquivos(fcli, ffunc, ftreino, fexerc);
                trocarParaArquivosOrdenados(&fcli, &ffunc, &ftreino, &fexerc);
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }

    return EXIT_FAILURE;
}