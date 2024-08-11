#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../kmldb/db.h"
#include "../utils/log.h"


// Calcula o tamanho da estrutura TFunc
int TFunc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char) * BF_NOME 
         + sizeof(char) * BF_CPF 
         + sizeof(char) * BF_EMAIL 
         + sizeof(char) * BF_TELEFONE 
         + sizeof(char) * BF_DATA_NASCIMENTO 
         + sizeof(double);
}

// Cria uma nova instância de TFunc
TFunc TFunc_New(unsigned long pk, const char* nome, const char* cpf, const char* email, const char* telefone, const char* data_nascimento, double salario) {
    TFunc func = {0};

    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BF_NOME || strlen(cpf) >= BF_CPF || strlen(email) >= BF_EMAIL || strlen(telefone) >= BF_TELEFONE || strlen(data_nascimento) >= BF_DATA_NASCIMENTO) {
        perror("TFunc buffer overflow");
        return func;
    }

    // Dependendo do ambiente, usa strncpy_s ou strncpy
    #ifdef _MSC_VER
        // Ambiente Microsoft
        func.pk = pk;
        strncpy_s(func.nome, sizeof(func.nome), nome, _TRUNCATE);
        strncpy_s(func.cpf, sizeof(func.cpf), cpf, _TRUNCATE);
        strncpy_s(func.email, sizeof(func.email), email, _TRUNCATE);
        strncpy_s(func.telefone, sizeof(func.telefone), telefone, _TRUNCATE);
        strncpy_s(func.data_nascimento, sizeof(func.data_nascimento), data_nascimento, _TRUNCATE);
    #else
        // Ambiente Unix/Linux
        func.pk = pk;
        strncpy(func.nome, nome, BF_NOME - 1);
        func.nome[BF_NOME - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.cpf, cpf, BF_CPF - 1);
        func.cpf[BF_CPF - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.email, email, BF_EMAIL - 1);
        func.email[BF_EMAIL - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.telefone, telefone, BF_TELEFONE - 1);
        func.telefone[BF_TELEFONE - 1] = '\0'; // Garantir que a string está terminada

        strncpy(func.data_nascimento, data_nascimento, BF_DATA_NASCIMENTO - 1);
        func.data_nascimento[BF_DATA_NASCIMENTO - 1] = '\0'; // Garantir que a string está terminada
    #endif

    func.salario = salario;
    return func;
}

// Função para buscar um funcionário pelo ID na tabela
TFunc TFunc_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TFunc funcEmpty = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do fileuivo");
        return funcEmpty;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return funcEmpty;
    }

    // Busca binária
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size;

    // Iniciar log e medir tempo
    FILE *log_file = init_log();
    clock_t start_time = clock();

    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        size_t seek = start_offset + (middle * size);
        fseek(file, seek, SEEK_SET);
        TFunc func;
        fread(&func, sizeof(TFunc), 1, file);
        // Registrar tentativa no log
        log_attempt(log_file, middle, func.pk);

        if (func.pk == pk) {
            log_total_time(log_file, start_time);
            fclose(log_file);  // Fecha o arquivo de log
            return func;
        }
        if (func.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    // Registra o tempo final e calcula a duração
    log_total_time(log_file, start_time);
    fclose(log_file);  // Fecha o arquivo de log

    return funcEmpty;
}

// Função para ler um funcionário de um fileuivo
TFunc TFunc_ReadReg(FILE *file) {
    TFunc func = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("fileuivo não pode ser NULL");
        return func;
    }
    if (fread(&func.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return func;
    }
    fread(func.nome, sizeof(char), sizeof(func.nome), file);
    fread(func.cpf, sizeof(char), sizeof(func.cpf), file);
    fread(func.email, sizeof(char), sizeof(func.email), file);
    fread(func.telefone, sizeof(char), sizeof(func.telefone), file);
    fread(func.data_nascimento, sizeof(char), sizeof(func.data_nascimento), file);
    fread(&func.salario, sizeof(double), 1, file);
    return func;
}

// Função para imprimir os dados de um funcionário
void TFunc_Print(TFunc *func) {
    if (func == NULL) {
        perror("Funcionário nulo\n");
        return;
    }
    printf("# Funcionario:\n");
    printf("| PK: %lu\n", func->pk);
    printf("| Nome: %s\n", func->nome);
    printf("| CPF: %s\n", func->cpf);
    printf("| Email: %s\n", func->email);
    printf("| Telefone: %s\n", func->telefone);
    printf("| Data de Nascimento: %s\n", func->data_nascimento);
    printf("| Salario: %.2lf\n", func->salario);
}

void TFunc_PrintGeneric(void* member) {
    TFunc* func = (TFunc*)member;
    printf("# Funcionário:\n");
    printf("| PK: %lu\n", func->pk);
    printf("| Nome: %s\n", func->nome);
    printf("| CPF: %s\n", func->cpf);
    printf("| Email: %s\n", func->email);
    printf("| Telefone: %s\n", func->telefone);
    printf("| Data de Nascimento: %s\n", func->data_nascimento);
    printf("| Salario: %.2lf\n", func->salario);
}

int TFuncClassificacaoInterna(FILE *file, const char* table_name) {
    rewind(file);

    int M = 100;
    DatabaseHeader header;
    int qtdParticoes = 0;
    char nomeParticao[256];

    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return -1;
    }

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return -1;
    }

    if (header.tables[index].size != sizeof(TFunc)) {
        perror("Tamanho do membro incompatível com a tabela");
        return -1;
    }

    int start_offset = header.tables[index].start_offset;
    int end_offset = header.tables[index].end_offset;
    int nFunc = (end_offset - start_offset) / sizeof(TFunc);

    fseek(file, start_offset, SEEK_SET);

    while (ftell(file) < end_offset) {
        TFunc *v[M];
        int i = 0;

        while (ftell(file) < end_offset && i < M) {
            v[i] = malloc(sizeof(TFunc));
            if (fread(v[i], sizeof(TFunc), 1, file) == 1) {
                i++;
            } else {
                free(v[i]);
                break;
            }
        }

        if (i != M) {
            M = i;
        }

        for (int j = 1; j < M; j++) {
            TFunc *t = v[j];
            int k = j - 1;
            while (k >= 0 && v[k]->pk > t->pk) {
                v[k + 1] = v[k];
                k--;
            }
            v[k + 1] = t;
        }

        snprintf(nomeParticao, sizeof(nomeParticao), "data/particions/particion_%d.dat", qtdParticoes);
        FILE *p = fopen(nomeParticao, "wb+");
        if (p == NULL) {
            perror("Erro ao criar arquivo de partição");
            for (int jj = 0; jj < M; jj++) {
                free(v[jj]);
            }
            return -1;
        }

        for (int i = 0; i < M; i++) {
            if (fwrite(v[i], sizeof(TFunc), 1, p) != 1) {
                perror("Erro ao gravar registro na partição");
                fclose(p);
                for (int jj = 0; jj < M; jj++) {
                    free(v[jj]);
                }
                return -1;
            }
            free(v[i]);
        }

        fclose(p);
        qtdParticoes++;
    }

    return qtdParticoes;
}




// Função para realizar a intercalação das partições geradas por seleção com substituição
int TFuncIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    typedef struct vetor {
        TFunc func;
        FILE *aux;
    } TVet;

    int fim = 0;
    char nome_file[256];

    TVet *v = malloc(num_particoes * sizeof(TVet));
    if (v == NULL) {
        perror("Erro ao alocar memória para vetores de partições");
        return -1;
    }

    for (int i = 0; i < num_particoes; i++) {
        snprintf(nome_file, sizeof(nome_file), "data/particions/particion_%d.dat", i);

        v[i].aux = fopen(nome_file, "rb");
        if (v[i].aux != NULL) {
            fread(&v[i].func, sizeof(TFunc), 1, v[i].aux);
            if (v[i].func.pk == 0) {
                v[i].func.pk = ULONG_MAX;
            }
        } else {
            fim = 1;
        }
    }

    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    while (!fim) {
        unsigned long menor = ULONG_MAX;
        int pos_menor = -1;

        for (int i = 0; i < num_particoes; i++) {
            if (v[i].func.pk < menor) {
                menor = v[i].func.pk;
                pos_menor = i;
            }
        }

        if (pos_menor == -1) {
            fim = 1;
        } else {
            fwrite(&v[pos_menor].func, sizeof(TFunc), 1, file);
            if (fread(&v[pos_menor].func, sizeof(TFunc), 1, v[pos_menor].aux) != 1) {
                v[pos_menor].func.pk = ULONG_MAX;
            }
        }
    }

    for (int i = 0; i < num_particoes; i++) {
        if (v[i].aux != NULL) {
            fclose(v[i].aux);
        }
    }

    free(v);
    return 0;
}
