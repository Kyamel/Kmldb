#include "exercicio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../kmldb/db.h"

// Calcula o tamanho da estrutura TExerc
int TExerc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char)*BE_NOME 
         + sizeof(char)*BE_TIPO 
         + sizeof(int);
}

// Cria uma nova instância de TExerc
TExerc TExerc_New(unsigned long pk, const char* nome, const char* tipo, int duration) {
    TExerc exec = {0};
    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BE_NOME || strlen(tipo) >= BE_TIPO) {
        perror("TExerc buffer overflow");
        return exec;
    }
    // Copiar os dados para a estrutura TExerc usando strncpy_s
    exec.pk = pk;
    strncpy_s(exec.nome, sizeof(exec.nome), nome, _TRUNCATE);
    strncpy_s(exec.tipo, sizeof(exec.tipo), tipo, _TRUNCATE);
    exec.duration = duration;
    return exec;
}

// Função para buscar uma execução pelo ID na tabela
TExerc TExerc_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TExerc exec = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return exec;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        fprintf_s(stderr, "Erro: Tabela '%s' não encontrada.\n", table_name);
        return exec;
    }
    // binary search
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size;
    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        size_t seek = start_offset + (middle * size);
        fseek(file, seek, SEEK_SET);
        exec = TExerc_ReadReg(file);
        if (exec.pk == pk) {
            return exec;
        }
        if (exec.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    return exec;
}

// Função para ler uma execução de um arquivo
TExerc TExerc_ReadReg(FILE *file) {
    TExerc exec = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return exec;
    }
    if (fread(&exec.pk, sizeof(unsigned long), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return exec;
    }
    fread(exec.nome, sizeof(char), sizeof(exec.nome), file);
    fread(exec.tipo, sizeof(char), sizeof(exec.tipo), file);
    fread(&exec.duration, sizeof(int), 1, file);
    return exec;
}

// Função para imprimir os dados de uma execução
void TExerc_Print(TExerc *exec) {
    if (exec == NULL) {
        perror("Exercicio nulo\n");
        return;
    }
    printf("# Exercicio:\n");
    printf("| PK: %lu\n", exec->pk);
    printf("| Nome: %s\n", exec->nome);
    printf("| Tipo: %s\n", exec->tipo);
    printf("| Duracao: %d segundos\n", exec->duration);
}

void TExerc_PrintGeneric(void* member) {
    TExerc* exerc = (TExerc*)member;
    printf("# Exercício:\n");
    printf("| PK: %lu\n", exerc->pk);
    printf("| Nome: %s\n", exerc->nome);
    printf("| Categoria: %s\n", exerc->tipo);
    printf("| Duracao: %d\n", exerc->duration);
}


int TExercClassificacaoInterna(FILE *file, const char* table_name) {
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

    if (header.tables[index].size != sizeof(TExerc)) {
        perror("Tamanho do membro incompatível com a tabela");
        return -1;
    }

    int start_offset = header.tables[index].start_offset;
    int end_offset = header.tables[index].end_offset;

    fseek(file, start_offset, SEEK_SET);

    while (ftell(file) < end_offset) {
        TExerc *v[M];
        int i = 0;

        while (ftell(file) < end_offset && i < M) {
            v[i] = malloc(sizeof(TExerc));
            if (fread(v[i], sizeof(TExerc), 1, file) == 1) {
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
            TExerc *t = v[j];
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
            if (fwrite(v[i], sizeof(TExerc), 1, p) != 1) {
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

int TExercIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    typedef struct vetor {
        TExerc exerc;
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
        rewind(v[i].aux);
        if (v[i].aux != NULL) {
            fread(&v[i].exerc, sizeof(TExerc), 1, v[i].aux);
        } else {
            fim = 1;
        }
    }

    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    while (!fim) {
        unsigned long menor = ULONG_MAX;
        int pos_menor;

        for (int i = 0; i < num_particoes; i++) {
            if (v[i].exerc.pk < menor) {
                menor = v[i].exerc.pk;
                pos_menor = i;
            }
        }

        if (menor == ULONG_MAX) {
            fim = 1;
        } else {
            fwrite(&v[pos_menor].exerc, sizeof(TExerc), 1, file);
            if (fread(&v[pos_menor].exerc, sizeof(TExerc), 1, v[pos_menor].aux) != 1) {
                v[pos_menor].exerc.pk = ULONG_MAX;
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
