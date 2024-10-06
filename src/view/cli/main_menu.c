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
#include "../../utils/createBase.h"

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

    printf("Data de Expiracao (DD-MM-AAAA): ");
    fgets(exp_date, sizeof(exp_date), stdin);
    exp_date[strcspn(exp_date, "\n")] = 0;

    TCliente cliente = TCliente_New(0, nome, cpf, email, telefone, exp_date, 0, 0);

    int err = cdbAdd(file, CLIENTES, &cliente, sizeof(TCliente), offsetof(TCliente, pk), offsetof(TCliente, next_pk), generic_write);

    if (err < 0) {
        printf("Erro ao cadastrar cliente. (rr: %d)\n", err);
    } else {
         printf("Cliente cadastrado com sucesso!\n");
    }
}

void buscarCliente(FILE *file) {
    printf("Buscar Cliente:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();



    TCliente cliente;
    cliente.pk = pk;
    int err = cdbFind(file, CLIENTES, &cliente, sizeof(TCliente), offsetof(TCliente, pk), offsetof(TCliente, next_pk), offsetof(TCliente, status));

    if (err < 0) {
        printf("Cliente nao encontrado. (err: %d)\n", err);
    } else {
        TCliente_PrintGeneric(&cliente);
    }
}

void updateCliente(FILE *file) {
    printf("Atualizar Cliente:\n");
    char nome[BC_NOME], cpf[BC_CPF], email[BC_EMAIL], telefone[BC_TELEFONE], exp_date[BC_EXP_DATE];

    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TCliente cliente;
    cliente.pk = pk;
    int err = cdbFind(file, CLIENTES, &cliente, sizeof(TCliente), offsetof(TCliente, pk), offsetof(TCliente, next_pk), offsetof(TCliente, status));

    if (err < 0) {
        printf("Cliente nao encontrado. (err: %d)\n", err);
    } else {
        TCliente_PrintGeneric(&cliente);
        return;
    }

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

    cliente = TCliente_New(0, nome, cpf, email, telefone, exp_date, 0, 0);

    err = cdbUpdate(file, CLIENTES, pk, &cliente, sizeof(TCliente), offsetof(TCliente, pk),
        offsetof(TCliente, next_pk), offsetof(TCliente, status), generic_write);

    if (err < 0) {
        printf("Erro ao atualizar cliente. (err: %d)\n", err);
    } else {
         printf("Cliente atualizado com sucesso!\n");
    }
}

void deletarCliente(FILE *file) {
    printf("Deletar Cliente:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TCliente cliente;
    cliente.pk = pk;
    int err = cdbDelete(file, CLIENTES, pk, sizeof(TCliente), offsetof(TCliente, pk),
        offsetof(TCliente, next_pk), offsetof(TCliente, status), generic_write);

    if (err < 0) {
        printf("Erro ao deletar cliente. (err: %d)\n", err);
    } else {
         printf("Cliente deletado com sucesso!\n");
    }
}

void cadastrarFuncionario(FILE *file) {
    char nome[BF_NOME], cpf[BF_CPF], email[BF_EMAIL], telefone[BF_TELEFONE], data_nascimento[BF_DATA_NASCIMENTO];
    char password[BF_PASSWORD];
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

    printf("Senha: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    printf("Salário: ");
    scanf_s("%lf", &salario);
    cClearInputBuffer();

    TFunc funcionario = TFunc_New(0, nome, cpf, email, telefone, data_nascimento, salario, password, 0, 0);

    int err = cdbAdd(file, FUNCIONARIOS, &funcionario, sizeof(TFunc), offsetof(TFunc, pk), offsetof(TFunc, next_pk), generic_write);

    if (err < 0) {
        printf("Erro ao cadastrar funcionário. (err: %d)\n", err);
    } else {
        printf("Funcionário cadastrado com sucesso!\n");
    }
}

void buscarFuncionario(FILE *file) {
    printf("Buscar Funcionário:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TFunc funcionario;
    funcionario.pk = pk;

    int err = cdbFind(file, FUNCIONARIOS, &funcionario, sizeof(TFunc), offsetof(TFunc, pk), offsetof(TFunc, next_pk), offsetof(TFunc, status));

    if (err < 0)  {
        printf("Funcionário não encontrado. (err: %d)\n", err);
    } else {
        TFunc_PrintGeneric(&funcionario);
    }
}

void updateFuncionario(FILE *file) {
    printf("Atualizar Funcionario:\n");
    char nome[BF_NOME], cpf[BF_CPF], email[BF_EMAIL], telefone[BF_TELEFONE],
        data_nascimento[BF_DATA_NASCIMENTO], password[BF_PASSWORD];

    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TFunc funcionario;
    funcionario.pk = pk;
    int err = cdbFind(file, FUNCIONARIOS, &funcionario, sizeof(TFunc), offsetof(TFunc, pk), offsetof(TFunc, next_pk), offsetof(TFunc, status));

    if (err < 0) {
        printf("Funcionario nao encontrado. (err: %d)\n", err);
        return;
    } else {
        TFunc_PrintGeneric(&funcionario);
    }

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

    printf("Data de Nascimento (DD-MM-AAAA): ");
    fgets(data_nascimento, sizeof(data_nascimento), stdin);
    data_nascimento[strcspn(data_nascimento, "\n")] = 0;

    printf("Senha: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    funcionario = TFunc_New(0, nome, cpf, email, telefone, data_nascimento, 2500.00, password, 0, 0);

    err = cdbUpdate(file, FUNCIONARIOS, pk, &funcionario, sizeof(TFunc), offsetof(TFunc, pk),
        offsetof(TFunc, next_pk), offsetof(TFunc, status), generic_write);

    if (err < 0) {
        printf("Erro ao atualizar funcionario. (err: %d)\n", err);
    } else {
         printf("Funcionario atualizado com sucesso!\n");
    }
}

void deletarFuncionario(FILE *file) {
    printf("Deletar Funcionario:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TFunc funcionario;
    funcionario.pk = pk;
    int err = cdbDelete(file, FUNCIONARIOS, pk, sizeof(TFunc), offsetof(TFunc, pk),
        offsetof(TFunc, next_pk), offsetof(TFunc, status), generic_write);

    if (err < 0) {
        printf("Erro ao deletar funcionario. (err: %d)\n", err);
    } else {
         printf("Funcionario deletado com sucesso!\n");
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

    TExerc exercicio = TExerc_New(0, nome, tipo, duration, 0, 0);

    int err = cdbAdd(file, EXERCICIOS, &exercicio, sizeof(TExerc), offsetof(TExerc, pk), offsetof(TExerc, next_pk), generic_write);

    if (err < 0) {
        printf("Erro ao cadastrar exercício. (err: %d)\n", err);
    } else {
        printf("Exercício cadastrado com sucesso!\n");
    }
}

void buscarExercicio(FILE *file) {
    printf("Buscar Exercicio:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TExerc exercicio;
    exercicio.pk = pk;

    int err = cdbFind(file, EXERCICIOS, &exercicio, sizeof(TExerc), offsetof(TExerc, pk), offsetof(TExerc, next_pk), offsetof(TExerc, status));

    if (err < 0) {
        printf("Exercício não encontrado. (err: %d)\n", err);
    } else {
        TExerc_PrintGeneric(&exercicio);
    }
}

void updateExercicio(FILE *file) {
    printf("Atualizar Exercicio:\n");
    char nome[BE_NOME], tipo[BE_TIPO];

    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TExerc exercicio;
    exercicio.pk = pk;
    int err = cdbFind(file, EXERCICIOS, &exercicio, sizeof(TExerc), offsetof(TExerc, pk), offsetof(TExerc, next_pk), offsetof(TExerc, status));

    if (err < 0) {
        printf("Exercicio nao encontrado. (err: %d)\n", err);
        return;
    } else {
        TExerc_PrintGeneric(&exercicio);
    }

    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    printf("Duracao (segundos): ");
    char durationStr[20];
    fgets(durationStr, sizeof(durationStr), stdin);
    durationStr[strcspn(durationStr, "\n")] = 0;

    // Avalia a expressão ou converte o número diretamente
    int duration = evaluateArithmeticExpression(durationStr);

    exercicio = TExerc_New(0, nome, tipo, duration, 0, 0);

    err = cdbUpdate(file, EXERCICIOS, pk, &exercicio, sizeof(TExerc), offsetof(TExerc, pk),
        offsetof(TExerc, next_pk), offsetof(TExerc, status), generic_write);

    if (err < 0) {
        printf("Erro ao atualizar exercicio. (err: %d)\n", err);
    } else {
         printf("Exercicio atualizado com sucesso!\n");
    }
}

void deletarExercicio(FILE *file) {
    printf("Deletar Exercicio:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TExerc exercicio;
    exercicio.pk = pk;
    int err = cdbDelete(file, EXERCICIOS, pk, sizeof(TExerc), offsetof(TExerc, pk),
        offsetof(TExerc, next_pk), offsetof(TExerc, status), generic_write);

    if (err < 0) {
        printf("Erro ao deletar exercicio. (err: %d)\n", err);
    } else {
         printf("Exercicio deletado com sucesso!\n");
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

    TExerc exercicio;
    exercicio.pk = epk;
    int err = cdbFind(fexerc, EXERCICIOS, &exercicio, sizeof(TExerc), offsetof(TExerc, pk), offsetof(TExerc, next_pk), offsetof(TExerc, status));
    if (err < 0) {
        printf("Exercicio nao encontrado. (err: %d)\n", err);
        return;
    }
    TExerc_PrintGeneric(&exercicio);

    TCliente cliente;
    cliente.pk = cpk;
    err = cdbFind(fcli, CLIENTES, &cliente, sizeof(TCliente), offsetof(TCliente, pk), offsetof(TCliente, next_pk), offsetof(TCliente, status));

    if (err < 0) {
        printf("Cliente nao encontrado. (err: %d)\n", err);
        return;
    }
    TCliente_PrintGeneric(&cliente);

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

    TTreino treino = TTreino_New(0, nome, tipo, cliente.pk, exercicio.pk, duration, 0, 0);

    err = cdbAdd(ftreino, TREINOS, &treino, sizeof(TTreino), offsetof(TTreino, pk), offsetof(TTreino, next_pk), generic_write);
    if (err < 0) {
        printf("Erro ao cadastrar treino. (err: %d)\n", err);
    } else{
        printf("Treino cadastrado com sucesso!\n");
    }
}

void buscarTreino(FILE *ftreino, FILE *fcli) {
    printf("Buscar Treino:\n");
    unsigned long cpk;

    printf("Cliente PK: ");
    scanf_s("%lu", &cpk);
    cClearInputBuffer();

    int err = cSearchPrintTreinoFullByCpk(ftreino, fcli, cpk);
    if (err < 0) {
        printf("Treino nao encontrado. (err: %d)\n", err);
    }
}

void updateTreino(FILE *file) {
    printf("Atualizar Treino:\n");
    char nome[BT_NOME], tipo[BT_TIPO];

    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TTreino treino;
    treino.pk = pk;
    int err = cdbFind(file, TREINOS, &treino, sizeof(TTreino), offsetof(TTreino, pk), offsetof(TTreino, next_pk), offsetof(TTreino, status));

    if (err < 0) {
        printf("Treino nao encontrado. (err: %d)\n", err);
        return;
    } else {
        TTreino_PrintGeneric(&treino);
    }

    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0;

    printf("Tipo: ");
    fgets(tipo, sizeof(tipo), stdin);
    tipo[strcspn(tipo, "\n")] = 0;

    treino = TTreino_New(0, nome, tipo, 1, 1, 45, 0, 0);

    err = cdbUpdate(file, TREINOS, pk, &treino, sizeof(TTreino), offsetof(TTreino, pk),
        offsetof(TTreino, next_pk), offsetof(TTreino, status), generic_write);

    if (err < 0) {
        printf("Erro ao atualizar treino. (err: %d)\n", err);
    } else {
         printf("Treino atualizado com sucesso!\n");
    }
}
void deletarTreino(FILE *file) {
    printf("Deletar Treino:\n");
    printf("PK: ");
    unsigned long pk;
    scanf_s("%lu", &pk);
    cClearInputBuffer();

    TTreino treino;
    treino.pk = pk;
    int err = cdbDelete(file, TREINOS, pk, sizeof(TTreino), offsetof(TTreino, pk),
        offsetof(TTreino, next_pk), offsetof(TTreino, status), generic_write);

    if (err < 0) {
        printf("Erro ao deletar treino. (err: %d)\n", err);
    } else {
         printf("Treino deletado com sucesso!\n");
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
    particions = TFuncSelecaoComSubstituicao(ffunc, FUNCIONARIOS);
    fseek(ffunc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ffunc);

    FILE *ffuncOrd = fopen(DB_FOLDER "/" FUNCIONARIOS ".dat", "w+b");
    if (ffuncOrd == NULL) {  // Verificar se o arquivo foi aberto corretamente
        printf("Erro ao abrir o arquivo %s: %d\n", DB_FOLDER "/" EXERCICIOS ".dat");
        return;
    }

    TFuncIntercalacaoComArvore(ffuncOrd, &header, particions);
    fclose(ffuncOrd);
    fclose(ffunc);
    progressoAtual++;
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Funcionarios");

    // CLIENTES
    printf("\nOrdenando clientes...\n");
    start_time = clock();
    particions = TClienteSelecaoComSubstituicao(fcli, CLIENTES);
    fseek(fcli, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fcli);

     // Declarar um ponteiro FILE
    FILE *fcliOrd = fopen(DB_FOLDER "/" CLIENTES ".dat", "w+b");
    if (fcliOrd == NULL) {  // Verificar se o arquivo foi aberto corretamente
        printf("Erro ao abrir o arquivo %s: %d\n", DB_FOLDER "/" EXERCICIOS ".dat");
        return;
    }

    TClienteIntercalacaoComArvore(fcliOrd, &header, particions);
    fclose(fcliOrd);
    fclose(fcli);
    progressoAtual++;
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Clientes");

    // EXERCICIOS
    printf("\nOrdenando exercicios...\n");
    start_time = clock();
    particions = TExercSelecaoComSubstituicao(fexerc, EXERCICIOS);
    fseek(fexerc, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, fexerc);


    FILE *fexercOrd = fopen(DB_FOLDER "/" EXERCICIOS ".dat", "w+b");
    if (fexercOrd == NULL) {  // Verificar se o arquivo foi aberto corretamente
        printf("Erro ao abrir o arquivo %s: %d\n", DB_FOLDER "/" EXERCICIOS ".dat");
        return;
    }

    TExercIntercalacaoComArvore(fexercOrd, &header, particions);
    fclose(fexercOrd);
    fclose(fexerc);
    progressoAtual++;
    end_time = clock();
    time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    atualizarBarraProgresso(progressoAtual, totalArquivos, time_spent, "Exercicios");

    // TREINOS
    printf("\nOrdenando treinos...\n");
    start_time = clock();
    particions = TTreinoSelecaoComSubstituicao(ftreino, TREINOS);
    fseek(ftreino, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, ftreino);

    FILE *ftreinoOrd = fopen(DB_FOLDER "/" TREINOS ".dat", "w+b");
    if (ftreinoOrd == NULL) {  // Verificar se o arquivo foi aberto corretamente
        printf("Erro ao abrir o arquivo %s: %d\n", DB_FOLDER "/" EXERCICIOS ".dat");
        return;
    }
    TTreinoIntercalacaoComArvore(ftreinoOrd, &header, particions);
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
    *fcli = cdbInit(DB_FOLDER"/" CLIENTES".dat");
    *ffunc = cdbInit(DB_FOLDER"/" FUNCIONARIOS".dat");
    *ftreino = cdbInit(DB_FOLDER"/" TREINOS".dat");
    *fexerc = cdbInit(DB_FOLDER"/" EXERCICIOS".dat");

    // Recarregar as tabelas se necessário
    cInitTables(*fcli, *ffunc, *ftreino, *fexerc);
}

void submenuCliente(FILE *fcli) {
    int opcao;
    while (1) {
        printf("\nMenu Cliente:\n");
        printf("1. Cadastrar Cliente\n");
        printf("2. Pesquisar Cliente\n");
        printf("3. Atualizar Cliente\n");
        printf("4. Deletar Cliente\n");
        printf("5. Exibir Todos os Clientes\n");
        printf("9. Voltar\n");
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
                updateCliente(fcli);
                break;
            case 4:
                deletarCliente(fcli);
                break;
            case 5:
                cdbReadAll(fcli, CLIENTES, offsetof(TCliente, pk), offsetof(TCliente, next_pk), sizeof(TCliente), TCliente_PrintGeneric);
                break;
            case 9:
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

void submenuFuncionario(FILE *ffunc) {
    int opcao;
    while (1) {
        printf("\nMenu Funcionario:\n");
        printf("1. Cadastrar Funcionario\n");
        printf("2. Pesquisar Funcionario\n");
        printf("3. Atualizar Funcionario\n");
        printf("4. Deletar Funcionario\n");
        printf("5. Exibir Todos os Funcionarios\n");
        printf("9. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1:
                cadastrarFuncionario(ffunc);
                break;
            case 2:
                buscarFuncionario(ffunc);
                break;
            case 3:
                updateFuncionario(ffunc);  // Função que você deve criar para atualizar
                break;
            case 4:
                deletarFuncionario(ffunc);  // Função que você deve criar para deletar
                break;
            case 5:
                cdbReadAll(ffunc, FUNCIONARIOS, offsetof(TFunc, pk), offsetof(TFunc, next_pk), sizeof(TFunc), TFunc_PrintGeneric);
                break;
            case 9:
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

void submenuExercicio(FILE *fexerc) {
    int opcao;
    while (1) {
        printf("\nMenu Exercicio:\n");
        printf("1. Cadastrar Exercicio\n");
        printf("2. Pesquisar Exercicio\n");
        printf("3. Atualizar Exercicio\n");
        printf("4. Deletar Exercicio\n");
        printf("5. Exibir Todos os Exercicios\n");
        printf("9. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1:
                cadastrarExercicio(fexerc);
                break;
            case 2:
                buscarExercicio(fexerc);
                break;
            case 3:
                updateExercicio(fexerc);  // Função que você deve criar para atualizar
                break;
            case 4:
                deletarExercicio(fexerc);  // Função que você deve criar para deletar
                break;
            case 5:
                cdbReadAll(fexerc, EXERCICIOS, offsetof(TExerc, pk), offsetof(TExerc, next_pk), sizeof(TExerc), TExerc_PrintGeneric);
                break;
            case 9:
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

void submenuTreino(FILE *ftreino, FILE *fexerc, FILE *fcli) {
    int opcao;
    while (1) {
        printf("\nMenu Treino:\n");
        printf("1. Cadastrar Treino\n");
        printf("2. Pesquisar Treino\n");
        printf("3. Atualizar Treino\n");
        printf("4. Deletar Treino\n");
        printf("5. Exibir Todos os Treinos\n");
        printf("9. Voltar\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1:
                cadastrarTreino(ftreino, fexerc, fcli);
                break;
            case 2:
                buscarTreino(ftreino, fexerc);
                break;
            case 3:
                updateTreino(ftreino);  // Função que você deve criar para atualizar
                break;
            case 4:
                deletarTreino(ftreino);  // Função que você deve criar para deletar
                break;
            case 5:
                cdbReadAll(ftreino, TREINOS, offsetof(TTreino, pk), offsetof(TTreino, next_pk), sizeof(TTreino), TTreino_PrintGeneric);
                break;
            case 9:
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

void subMenuDebug(FILE **fcli, FILE **ffunc, FILE **ftreino, FILE **fexerc) {
    int opcao;
    while (1) {
        printf("\nDebug:\n");
        printf("1. Criar Base de Teste\n");
        printf("2. Trocar para Base de Testes\n");
        printf("3. Retornar à Base Normal\n");
        printf("4. Deletar Base de Testes\n");
        printf("9. Voltar ao Menu Principal\n");

        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1: {
                printf("Criando base de teste...\n");
                int size, trocas;
                printf("Tamanho da base: ");
                scanf_s("%d", &size);
                printf("Quantidade de trocas: ");
                scanf_s("%d", &trocas);
                createBase(size, trocas);
                break;
            }
            case 2: {
                printf("Trocando para a base de testes...\n");
                // Fechando arquivos atuais
                cCloseDatabase(*fcli, *ffunc, *ftreino, *fexerc);
                // Reabrindo arquivos da pasta de testes
                *fcli = cdbInit(DB_FOLDER "/testes/" CLIENTES ".dat");
                *ffunc = cdbInit(DB_FOLDER "/testes/" FUNCIONARIOS ".dat");
                *ftreino = cdbInit(DB_FOLDER "/testes/" TREINOS ".dat");
                *fexerc = cdbInit(DB_FOLDER "/testes/" EXERCICIOS ".dat");
                printf("Base de testes carregada com sucesso!\n");
                break;
            }
            case 3: {
                printf("Retornando à base principal...\n");
                // Fechando arquivos de testes
                cCloseDatabase(*fcli, *ffunc, *ftreino, *fexerc);
                // Reabrindo arquivos da pasta original
                *fcli = cdbInit(DB_FOLDER "/" CLIENTES ".dat");
                *ffunc = cdbInit(DB_FOLDER "/" FUNCIONARIOS ".dat");
                *ftreino = cdbInit(DB_FOLDER "/" TREINOS ".dat");
                *fexerc = cdbInit(DB_FOLDER "/" EXERCICIOS ".dat");
                printf("Base normal carregada com sucesso!\n");
                break;
            }
            case 4: {
                printf("Deletando base de testes...\n");
                remove(DB_FOLDER "/testes/" CLIENTES ".dat");
                remove(DB_FOLDER "/testes/" FUNCIONARIOS ".dat");
                remove(DB_FOLDER "/testes/" TREINOS ".dat");
                remove(DB_FOLDER "/testes/" EXERCICIOS ".dat");
                printf("Base de testes deletada.\n");
                break;
            }
            case 9:
                return; // Volta ao menu principal
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }
}

int cli_main_menu() {

    FILE *fcli;
    FILE *ffunc;
    FILE *ftreino;
    FILE *fexerc;

    // Inicializa arquivos de banco de dados
    fcli = cdbInit(DB_FOLDER"/" CLIENTES".dat");
    ffunc = cdbInit(DB_FOLDER"/" FUNCIONARIOS".dat");
    ftreino = cdbInit(DB_FOLDER"/" TREINOS".dat");
    fexerc = cdbInit(DB_FOLDER"/" EXERCICIOS".dat");

    cInitTables(fcli, ffunc, ftreino, fexerc);
    cdbWelcome();

    int opcao;

    while (1) {
        printf("\nMenu Principal:\n");
        printf("1. Menu Cliente\n");
        printf("2. Menu Funcionario\n");
        printf("3. Menu Exercicio\n");
        printf("4. Menu Treino\n");
        printf("5. Debug\n");
        printf("9. Sair\n");
        printf("Escolha uma opcao: ");
        scanf_s("%d", &opcao);
        cClearInputBuffer();

        switch (opcao) {
            case 1:
                submenuCliente(fcli);
                break;
            case 2:
                submenuFuncionario(ffunc);
                break;
            case 3:
                submenuExercicio(fexerc);
                break;
            case 4:
                submenuTreino(ftreino, fexerc, fcli);
                break;
            case 5:
                // Opcional: Adicionar funcionalidades de debug no futuro
                subMenuDebug(&fcli, &ffunc, &ftreino, &fexerc);
                break;
            case 9: {
                int ok = cCloseDatabase(fcli, ffunc, ftreino, fexerc);
                return ok;
            }
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    }

    return EXIT_FAILURE;
}
