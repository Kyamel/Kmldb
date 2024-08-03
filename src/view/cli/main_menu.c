#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../controler/db_wrapper.h"
#include "../controler/crud.h"

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

#define DB_FOLDER "./data"

// Funções auxiliares
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

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

    addCliente(file, "clientes.dat", nome, cpf, email, telefone, exp_date);
    printf("Cliente cadastrado com sucesso!\n");
}

void buscarCliente(FILE *file) {
    char cpf[BC_CPF];

    printf("Buscar Cliente:\n");
    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    TCliente cliente = TCliente_GetByPK(file, "clientes.dat", strtoul(cpf, NULL, 10));
    if (cliente.pk != 0) {
        printCliente(&cliente);
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

    printf("Salario: ");
    scanf_s("%lf", &salario);
    clearInputBuffer();

    addFunc(file, "funcionarios.dat", nome, cpf, email, telefone, data_nascimento, salario);
    printf("Funcionario cadastrado com sucesso!\n");
}

void buscarFuncionario(FILE *file) {
    char cpf[BF_CPF];

    printf("Buscar Funcionário:\n");
    printf("CPF: ");
    fgets(cpf, sizeof(cpf), stdin);
    cpf[strcspn(cpf, "\n")] = 0;

    TFunc func = TFunc_GetByPK(file, "funcionarios.dat", strtoul(cpf, NULL, 10));
    if (func.pk != 0) {
        printFunc(&func);
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

    printf("Duracao (segundos): ");
    scanf_s("%d", &duration);
    clearInputBuffer();

    addExerc(file, "exercicios.dat", nome, tipo, duration);
    printf("Exercicio cadastrado com sucesso!\n");
}

void buscarExercicio(FILE *file) {
    char nome[BE_NOME];

    printf("Buscar Exercicio:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    TExerc exerc = readExerc(file, "exercicios.dat");
    if (strcmp(exerc.nome, nome) == 0) {
        printExerc(&exerc);
    } else {
        printf("Exercicio nao encontrado.\n");
    }
}

void cadastrarTreino(FILE *file, FILE *fexerc, FILE *fcli) {
    char nome[BT_NOME], tipo[BT_TIPO];
    int cpk, epk;

    printf("Cadastro de Treino:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    printf("PK do Cliente: ");
    scanf_s("%d", &cpk);
    clearInputBuffer();

    printf("PK do Exercicio: ");
    scanf_s("%d", &epk);
    clearInputBuffer();

    addTreinoDoC(file, "treinos.dat", nome, tipo, epk, cpk);
    printf("Treino cadastrado com sucesso!\n");
}

void buscarTreino(FILE *file) {
    char nome[BT_NOME];

    printf("Buscar Treino:\n");
    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    TTreino treino = readTreino(file, "treinos.dat");
    if (strcmp(treino.nome, nome) == 0) {
        printTreino(&treino);
    } else {
        printf("Treino nao encontrado.\n");
    }
}

int main_menu() {
    FILE *fcli = cInit(DB_FOLDER"/clientes.dat");
    FILE *ffunc = cInit(DB_FOLDER"/funcionarios.dat");
    FILE *ftreino = cInit(DB_FOLDER"/treinos.dat");
    FILE *fexec = cInit(DB_FOLDER"/exercicios.dat");

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
        clearInputBuffer();

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
                cadastrarExercicio(fexec);
                break;
            case 6:
                buscarExercicio(fexec);
                break;
            case 7:
                cadastrarTreino(ftreino, fexec, fcli);
                break;
            case 8:
                buscarTreino(ftreino);
                break;
            case 9:
                printf("Cliente ");
                cClose(fcli);
                printf("Funcionario ");
                cClose(ffunc);
                printf("Treino ");
                cClose(ftreino);
                printf("Exercicio ");
                cClose(fexec);
                printf("Saindo... Pressione ENTER\n");
                clearInputBuffer();
                exit(EXIT_SUCCESS);
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }

    return EXIT_SUCCESS;
}
