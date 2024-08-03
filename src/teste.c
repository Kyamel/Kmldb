#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../controler/db_wrapper.h"
#include "../controler/crud.h"

#include "../model/exercicio.h"
#include "../model/cliente.h"
#include "../model/treino.h"
#include "../model/funcionario.h"

#define DB_FOLDER "./data" // Corrigido para o diretório correto

// Função para inicializar as tabelas
void initTables(FILE *fcli, FILE *ffunc, FILE *ftreino, FILE *fexec) {
    cdbCreateTable(fcli, "clientes.dat", sizeof(TCliente));
    cdbCreateTable(ffunc, "funcionarios.dat", sizeof(TFunc));
    cdbCreateTable(ftreino, "treinos.dat", sizeof(TTreino));
    cdbCreateTable(fexec, "exercicios.dat", sizeof(TExerc));
}

// Função principal para teste
int teste() {
    FILE *fcli = cdbInit(DB_FOLDER"/clientes.dat");
    FILE *ffunc = cdbInit(DB_FOLDER"/funcionarios.dat");
    FILE *ftreino = cdbInit(DB_FOLDER"/treinos.dat");
    FILE *fexec = cdbInit(DB_FOLDER"/exercicios.dat");

    // Inicializa as tabelas
    initTables(fcli, ffunc, ftreino, fexec);

    // Adiciona clientes
    cAddCliente(fcli, "clientes.dat", "João Silva", "12345678900", "joao@example.com", "123456789", "2025-12-31");
    cAddCliente(fcli, "clientes.dat", "Maria Oliveira", "98765432100", "maria@example.com", "987654321", "2024-05-20");

    // Adiciona funcionários
    cAddFunc(ffunc, "funcionarios.dat", "Carlos Santos", "11122233344", "carlos@example.com", "321654987", "1985-11-10", 3000.00);
    cAddFunc(ffunc, "funcionarios.dat", "Ana Costa", "55566677788", "ana@example.com", "456789123", "1990-04-25", 3500.00);

    // Adiciona exercícios
    TExerc exerc1 = TExerc_New("Flexão", "Peito", 30);
    TExerc exerc2 = TExerc_New("Agachamento", "Perna", 45);
    CAddExerc(fexec, "exercicios.dat", exerc1.nome, exerc1.tipo, exerc1.duration);
    CAddExerc(fexec, "exercicios.dat", exerc2.nome, exerc2.tipo, exerc2.duration);

    // Adiciona treinos
    TCliente cliente1 = TCliente_New("João Silva", "12345678900", "joao@example.com", "123456789", "2025-12-31");
    TCliente cliente2 = TCliente_New("Maria Oliveira", "98765432100", "maria@example.com", "987654321", "2024-05-20");
    cAddTreinoNotC(ftreino, "treinos.dat", "Treino de Peito", "Hipertrofia", &exerc1, &cliente1);
    cAddTreinoNotC(ftreino, "treinos.dat", "Treino de Perna", "Força", &exerc2, &cliente2);

    // Leitura e impressão dos registros
    printf("Leitura e impressão dos registros:\n");

    // Imprime clientes
    TCliente cliente;
    cliente = cReadCliente(fcli, "clientes.dat", 1);
    cPrintCliente(&cliente);
    
    // Imprime funcionários
    TFunc func;
    func = cReadFunc(ffunc, "funcionarios.dat", 1);
    cPrintFunc(&func);

    // Imprime exercícios
    TExerc exerc;
    exerc = cReadExerc(fexec, "exercicios.dat", 1);
    cPrintExerc(&exerc);

    // Imprime treinos
    TTreino treino;
    treino = cReadTreino(ftreino, "treinos.dat", 1);
    cPrintTreino(&treino);

    // Fecha os arquivos
    cdbClose(fcli);
    cdbClose(ffunc);
    cdbClose(ftreino);
    cdbClose(fexec);

    return EXIT_SUCCESS;
}
