#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../controler/db_wrapper.h"
#include "../controler/crud.h"

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

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

    printf("Duração (segundos): ");
    scanf_s("%d", &duration);
    cClearInputBuffer();

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

void cadastrarTreino(FILE *file, FILE *fexerc, FILE *fcli) {
    char nome[BT_NOME], tipo[BT_TIPO];
    unsigned long cpk, epk;

    printf("Cadastro de Treino:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    printf("PK do Cliente: ");
    scanf_s("%lu", &cpk);
    cClearInputBuffer();

    printf("PK do Exercicio: ");
    scanf_s("%lu", &epk);
    cClearInputBuffer();

    cAddTreinoDoC(file, TREINOS, 0, nome, tipo, epk, cpk);
    printf("Treino cadastrado com sucesso!\n");
}

void buscarTreino(FILE *file) {
    printf("Buscar Treino:\n");
    unsigned long epk, cpk;
    printf("Exercicio PK: ");
    scanf_s("%lu", &epk);
    cClearInputBuffer();
    printf("Cliente PK: ");
    scanf_s("%lu", &cpk);
    cClearInputBuffer();

    TTreino treino = cSearchTreinoComp(file, TREINOS, epk, cpk);
    if (treino.pk != 0) {
        cPrintTreino(&treino);
    } else {
        printf("Treino não encontrado.\n");
    }
}

int cli_main_menu() {
    FILE *fcli = cdbInit(DB_FOLDER"/"CLIENTES".dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/"FUNCIONARIOS".dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/"TREINOS".dat");
    FILE *fexer = cdbInit(DB_FOLDER"/"EXERCICIOS".dat");

    cInitTables(fcli, ffunc, ftreino, fexer);

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
                cadastrarExercicio(fexer);
                break;
            case 6:
                buscarExercicio(fexer);
                break;
            case 7:
                cadastrarTreino(ftreino, fexer, fcli);
                break;
            case 8:
                buscarTreino(ftreino);
                break;
            case 9:
                cCloseDatabase(fcli, ffunc, ftreino, fexer);
                printf("Saindo... Pressione ENTER para sair.");
                cClearInputBuffer();
                return EXIT_SUCCESS;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }

    return EXIT_FAILURE;
}
