#include "exercicio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../kmldb/db.h"
#include "../utils/log.h"
#include "../utils/util.h"

// Calcula o tamanho da estrutura TExerc
int TExerc_Size() {
    return sizeof(long unsigned) 
         + sizeof(char)*BE_NOME 
         + sizeof(char)*BE_TIPO 
         + sizeof(int);
}

// Cria uma nova instância de TExerc
TExerc TExerc_New(unsigned long pk, const char* nome, const char* tipo, int duration, size_t next_pk, int status) {
    TExerc exec = {0};
    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BE_NOME || strlen(tipo) >= BE_TIPO) {
        perror("TExerc buffer overflow");
        return exec;
    }
    // Copiar os dados para a estrutura TExerc usando strncpy_s
    exec.pk = pk;
    exec.status = status;
    exec.next_pk = next_pk;
    strncpy_s(exec.nome, sizeof(exec.nome), nome, _TRUNCATE);
    strncpy_s(exec.tipo, sizeof(exec.tipo), tipo, _TRUNCATE);
    exec.duration = duration;
    return exec;
}

// Função para buscar uma execução pelo ID na tabela
TExerc TExerc_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TExerc execEmpty = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return execEmpty;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        fprintf_s(stderr, "Erro: Tabela '%s' não encontrada.\n", table_name);
        return execEmpty;
    }
    // binary search
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
        TExerc exec;
        fread(&exec, sizeof(TExerc), 1, file);
        // Registrar tentativa no log
        log_attempt(log_file, middle, exec.pk);

        if (exec.pk == pk) {
            log_total_time(log_file, start_time);
            fclose(log_file);  // Fecha o arquivo de log
            return exec;
        }
        if (exec.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
    // Registra o tempo final e calcula a duração
    log_total_time(log_file, start_time);
    fclose(log_file);  // Fecha o arquivo de log
    
    return execEmpty;
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


// Função para realizar a seleção com substituição como método de ordenação
int TExercSelecaoComSubstituicao(FILE *file, const char *table_name) {
    fflush(file);
    int ok = clearFolder("data/particions");
    if (ok == -1) return ok;

    int numeroDeParticoes = 0;
    unsigned long menorId;
    int comparacao = 0;

    int M = 6;
    TExerc exerc_vet[6]; // Array de M registros
    unsigned long auxVetPks[6] = {0}; // IDs dos registros
    int vetCongelado[6] = {0}; // Controle de congelamento

    DatabaseHeader header;
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

    // Ler M registros do arquivo para a memória
    fseek(file, header.tables[index].start_offset, SEEK_SET);
    for (int i = 0; i < M; ++i) {
        if (fread(&exerc_vet[i], sizeof(TExerc), 1, file) != 1) {
            auxVetPks[i] = ULONG_MAX;  // Marcar posição como vazia se não for possível ler
        } else {
            auxVetPks[i] = exerc_vet[i].pk; // Guardar o ID do registro
        }
    }

   /*  // Cria o diretório se não existir
    if (!checkIfFolderExist("src/partitions")) {
        _mkdir("src/partitions");
    } */

    // Loop principal para processar o arquivo de entrada
    while (1) {
        // Criar uma nova partição
        char nomeParticao[100];
        sprintf(nomeParticao, "data/particions/particion_%d.dat", numeroDeParticoes);
        FILE *filePartition = fopen(nomeParticao, "wb+");
        if (!filePartition) {
            perror("Erro ao criar a partição");
            return -1;
        }

        // Processar registros até que todos estejam congelados
        while (1) {
            menorId = ULONG_MAX;
            int posicaoMenorId = -1;

            // Encontrar o menor registro não congelado
            for (int i = 0; i < M; ++i) {
                if (!vetCongelado[i] && auxVetPks[i] < menorId) {
                    menorId = auxVetPks[i];
                    posicaoMenorId = i;
                }
            }

            // Se não houver mais registros não congelados, saia do loop
            if (posicaoMenorId == -1) break;

            // Gravar o registro com o menor ID na partição
            fwrite(&exerc_vet[posicaoMenorId], sizeof(TExerc), 1, filePartition);

            // Ler o próximo registro do arquivo
            TExerc novoFunc;
            if (fread(&novoFunc, sizeof(TExerc), 1, file) == 1) {
                // Substituir o registro no buffer
                exerc_vet[posicaoMenorId] = novoFunc;
                auxVetPks[posicaoMenorId] = novoFunc.pk;

                // Congelar se o novo ID for menor que o último gravado
                if (novoFunc.pk < menorId) {
                    vetCongelado[posicaoMenorId] = 1;
                }
            } else {
                // Se não houver mais registros, marcar como congelado e indicar fim da leitura
                auxVetPks[posicaoMenorId] = ULONG_MAX; // Indicar que não há mais registros
                vetCongelado[posicaoMenorId] = 1;
            }
        }

        // Fechar a partição atual
        fclose(filePartition);
        numeroDeParticoes++;

        // Verificar se todos os registros estão congelados
        int todosCongelados = 1;
        for (int i = 0; i < M; ++i) {
            if (auxVetPks[i] != ULONG_MAX) { // Se houver registros válidos não congelados
                todosCongelados = 0;
                break;
            }
        }

        // Se todos estão congelados, descongelar e iniciar nova partição
        if (todosCongelados) break;

        for (int i = 0; i < M; ++i) {
            vetCongelado[i] = 0;  // Descongelar todos os registros
        }
    }

    return numeroDeParticoes;
}

typedef struct vetor {
    TExerc exerc;
    FILE *aux;
} TVet;


// Função auxiliar para construir a árvore de vencedores
void TExercConstruirArvoreVencedores(TVet *v, int *arvore, int num_particoes) {
    // Inicializa a árvore de vencedores
    for (int i = 0; i < num_particoes; i++) {
        arvore[num_particoes + i] = i; // Folhas são as partições
    }

    // Constroi a árvore de vencedores
    for (int i = num_particoes - 1; i > 0; i--) {
        int esquerda = arvore[2 * i];
        int direita = arvore[2 * i + 1];
        arvore[i] = (v[esquerda].exerc.pk <= v[direita].exerc.pk) ? esquerda : direita;
    }
}

// Atualiza a árvore de vencedores após uma mudança em uma das folhas
void TExercAtualizarArvoreVencedores(TVet *v, int *arvore, int num_particoes, int pos) {
    pos += num_particoes;
    while (pos > 1) {
        pos /= 2;
        int esquerda = arvore[2 * pos];
        int direita = arvore[2 * pos + 1];
        arvore[pos] = (v[esquerda].exerc.pk <= v[direita].exerc.pk) ? esquerda : direita;
    }
}

int TExercIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    TVet *v = malloc(num_particoes * sizeof(TVet));
    if (v == NULL) {
        perror("Erro ao alocar memória para vetores de partições");
        return -1;
    }
    // O filho esquerdo está em 2 * i.
    // O filho direito está em 2 * i + 1.
    // O pai está em i / 2.
    int *arvore = malloc((2 * num_particoes) * sizeof(int));
    if (arvore == NULL) {
        perror("Erro ao alocar memória para a árvore de vencedores");
        free(v);
        return -1;
    }

    int fim = 0;
    char nome_file[256];

    for (int i = 0; i < num_particoes; i++) {
        snprintf(nome_file, sizeof(nome_file), "data/particions/particion_%d.dat", i);
        v[i].aux = fopen(nome_file, "rb");

        if (v[i].aux != NULL) {
            fread(&v[i].exerc, sizeof(TExerc), 1, v[i].aux);
            if (v[i].exerc.pk == 0) {
                v[i].exerc.pk = ULONG_MAX;
            }
        } else {
            v[i].exerc.pk = ULONG_MAX;  // Marca a partição como finalizada
        }
    }

    // Constroi a árvore de vencedores inicialmente
    TExercConstruirArvoreVencedores(v, arvore, num_particoes);

    // Reinicializa o arquivo de saída
    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    while (!fim) {
        int vencedor = arvore[1]; // A raiz da árvore de vencedores contém o índice do menor elemento

        if (v[vencedor].exerc.pk == ULONG_MAX) {
            fim = 1;  // Todas as partições acabaram
        } else {
            fwrite(&v[vencedor].exerc, sizeof(TExerc), 1, file);

            // Carrega o próximo elemento da partição vencedora
            if (fread(&v[vencedor].exerc, sizeof(TExerc), 1, v[vencedor].aux) != 1) {
                v[vencedor].exerc.pk = ULONG_MAX; // Marca a partição como finalizada
            }

            // Atualiza a árvore de vencedores com o novo elemento da partição vencedora
            TExercAtualizarArvoreVencedores(v, arvore, num_particoes, vencedor);
        }
    }

    // Fecha os arquivos auxiliares
    for (int i = 0; i < num_particoes; i++) {
        if (v[i].aux != NULL) {
            fclose(v[i].aux);
        }
    }

    free(v);
    free(arvore);
    return 0;
}
