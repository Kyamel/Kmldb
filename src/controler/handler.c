/* #include <stdio.h>
#include <string.h>
#include "crud.h"
#include "db_wrapper.h"

// Função de manipulação para diferentes operações
void handler(char op, char type, FILE *file, const char *table_name, ...) {
    va_list args;
    va_start(args, table_name);

    switch (op) {
        case 'C':  // Create
            if (type == 'f') {  // Funcionario
                const char *nome = va_arg(args, const char*);
                const char *cpf = va_arg(args, const char*);
                const char *email = va_arg(args, const char*);
                const char *tel = va_arg(args, const char*);
                const char *data_nascimento = va_arg(args, const char*);
                double salario = va_arg(args, double);
                addFunc(file, table_name, nome, cpf, email, tel, data_nascimento, salario);
            } else if (type == 'c') {  // Cliente
                const char *nome = va_arg(args, const char*);
                const char *cpf = va_arg(args, const char*);
                const char *email = va_arg(args, const char*);
                const char *tel = va_arg(args, const char*);
                const char *exp_date = va_arg(args, const char*);
                addCliente(file, table_name, nome, cpf, email, tel, exp_date);
            } else if (type == 'e') {  // Exercicio
                const char *nome = va_arg(args, const char*);
                const char *tipo = va_arg(args, const char*);
                int duration = va_arg(args, int);
                addExerc(file, table_name, nome, tipo, duration);
            } else if (type == 't') {  // Treino
                const char *nome = va_arg(args, const char*);
                const char *tipo = va_arg(args, const char*);
                long unsigned epk = va_arg(args, unsigned long);
                long unsigned cpk = va_arg(args, unsigned long);
                addTreinoDoC(file, table_name, nome, tipo, epk, cpk);
            }
            break;

        case 'R':  // Read
            if (type == 'f') {  // Funcionario
                long unsigned pk = va_arg(args, unsigned long);
                TFunc func = readFunc(file, table_name, pk);
                printFunc(&func);
            } else if (type == 'c') {  // Cliente
                long unsigned pk = va_arg(args, unsigned long);
                TCliente cliente = readCliente(file, table_name, pk);
                printCliente(&cliente);
            } else if (type == 'e') {  // Exercicio
                long unsigned pk = va_arg(args, unsigned long);
                TExerc exerc = readExerc(file, table_name, pk);
                printExerc(&exerc);
            } else if (type == 't') {  // Treino
                long unsigned pk = va_arg(args, unsigned long);
                TTreino treino = readTreino(file, table_name, pk);
                printTreino(&treino);
            }
            break;

        case 'U':  // Update
            // Implementar a lógica de atualização
            break;

        case 'D':  // Delete
            // Implementar a lógica de exclusão
            break;

        default:
            printf("Operação inválida.\n");
            break;
    }

    va_end(args);
}
 */