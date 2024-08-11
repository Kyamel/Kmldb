#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../controler/db_wrapper.h"
#include "../controler/crud.h"

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

#include "../../utils/util.h"

#include "main_menu.h"

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

    printf("Data de Expiração (AAAA-MM-DD): ");
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

    TCliente cliente = TCliente_GetByPK(file, CLIENTES, pk);
    if (cliente.pk != 0) {
        cPrintCliente(&cliente);
    } else {
        printf("Cliente não encontrado.\n");
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
        printf("Funcionário não encontrado.\n");
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
    printf("Exercício cadastrado com sucesso!\n");
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
        printf("Exercício não encontrado.\n");
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
        printf("Exercicio não encontrado.\n");
        return;
    }
    TExerc_Print(&exercicio);

    TCliente cliente = cSearchCliente(fcli, CLIENTES, cpk);
    if (cliente.pk <= 0) {
        printf("Cliente não encontrado.\n");
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

    cAddTreinoNotC(ftreino, TREINOS, 0, nome, tipo, &exercicio, &cliente, duration);
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
        printf("Treino não encontrado.\n");
    }
}

int cli_main_menu() {

    FILE *fcli;
    FILE *ffunc;
    FILE *ftreino;
    FILE *fexerc;

    if (1){
        // se true, seleciona a base de dados ordenada
        fcli = cdbInit(DB_FOLDER"/"CLIENTES"COrd.dat");
        ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS"COrd.dat");
        ftreino = cdbInit(DB_FOLDER"/"TREINOS"COrd.dat");
        fexerc = cdbInit(DB_FOLDER"/"EXERCICIOS"COrd.dat");
    } else {
        fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
        ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
        ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
        fexerc = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");
    }
    cInitTables(fcli, ffunc, ftreino, fexerc);

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
            case 9:
                int ok = cCloseDatabase(fcli, ffunc, ftreino, fexerc);
                return ok;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }

    return EXIT_FAILURE;
}
