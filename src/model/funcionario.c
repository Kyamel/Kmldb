#include "funcionario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../kmldb/db.h"
#include "../utils/log.h"
#include "../utils/util.h"
#include "../utils/binTree.h"


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
TFunc TFunc_New(unsigned long pk, const char* nome, const char* cpf, const char* email,
    const char* telefone, const char* data_nascimento, double salario,
    const char* password, int status, size_t next_pk) {

    TFunc func = {0};

    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BF_NOME || strlen(cpf) >= BF_CPF || strlen(email) >= BF_EMAIL
        || strlen(telefone) >= BF_TELEFONE || strlen(data_nascimento) >= BF_DATA_NASCIMENTO || strlen(password) >= HASH_SIZE) {
        perror("TFunc buffer overflow");
        return func;
    }

    func.pk = pk; // Inicialização comum

    #ifdef _MSC_VER
        // Ambiente Microsoft
        strncpy_s(func.nome, sizeof(func.nome), nome, _TRUNCATE);
        strncpy_s(func.cpf, sizeof(func.cpf), cpf, _TRUNCATE);
        strncpy_s(func.email, sizeof(func.email), email, _TRUNCATE);
        strncpy_s(func.telefone, sizeof(func.telefone), telefone, _TRUNCATE);
        strncpy_s(func.data_nascimento, sizeof(func.data_nascimento), data_nascimento, _TRUNCATE);
        strncpy_s(func.password, sizeof(func.password), password, _TRUNCATE);
    #else
        // Ambiente Unix/Linux
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

        strncpy(func.password, password, HASH_SIZE - 1);
        func.password[HASH_SIZE - 1] = '\0'; // Garantir que a string está terminada
    #endif

    func.salario = salario;
    func.status = status;
    func.next_pk = next_pk;

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
    fread(&func.password, sizeof(char), sizeof(func.password), file);
    fread(&func.status, sizeof(int), 1, file);
    fread(&func.next_pk, sizeof(int), 1, file);
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
    printf("| password: %s\n", func->password);
    printf("| status: %d\n", func->status);
    printf("| next_pk: %lu\n", func->next_pk);
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

// Função de comparação para qsort
int compararTFunc(const void *a, const void *b) {
    TFunc *funcA = *(TFunc **)a;
    TFunc *funcB = *(TFunc **)b;

    if (funcA->pk > funcB->pk) return 1;
    if (funcA->pk < funcB->pk) return -1;
    return 0;
}

// Função para realizar a intercalação das partições geradas por seleção com substituição
int TFuncIntercalacaoOtima(char nomeDaParticao[], int qtdParticoes) {
    if (qtdParticoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    FILE *particoes[qtdParticoes];
    TFunc *funcVet[qtdParticoes];

    for (int i = 0; i < qtdParticoes; i++) {
        char nomeArqParticao[100];
        snprintf(nomeArqParticao, sizeof(nomeArqParticao), "%s%d.dat", nomeDaParticao, i);
        particoes[i] = fopen(nomeArqParticao, "rb");

        if (!particoes[i]) {
            printf("Erro ao abrir o arquivo da partição %s\n", nomeArqParticao);
            return -1;
        }

        funcVet[i] = (TFunc *)malloc(sizeof(TFunc));
        if (!funcVet[i]) {
            perror("Erro ao alocar memória");
            return -1;
        }

        if (fread(funcVet[i], sizeof(TFunc), 1, particoes[i]) != 1) {
            if (feof(particoes[i])) {
                free(funcVet[i]);
                funcVet[i] = NULL;
            } else {
                perror("Erro ao ler o registro");
                free(funcVet[i]);
                return -1;
            }
        }
    }

    // Intercalação ótima em grupos de partições
    for (int grupo = 0; grupo < qtdParticoes; grupo += 4) {
        TFunc *grupoFunc[4];
        int idx = 0;

        // Carrega e intercala os registros do grupo
        for (int i = grupo; i < grupo + 4 && i < qtdParticoes; i++) {
            while (funcVet[i]) {
                grupoFunc[idx++] = funcVet[i];

                TFunc *func = (TFunc *)malloc(sizeof(TFunc));
                if (!func) {
                    perror("Erro ao alocar memória");
                    return -1;
                }

                if (fread(func, sizeof(TFunc), 1, particoes[i]) != 1) {
                    if (feof(particoes[i])) {
                        free(func);
                        funcVet[i] = NULL;
                    } else {
                        perror("Erro ao ler o registro");
                        free(func);
                        return -1;
                    }
                } else {
                    funcVet[i] = func;
                }
            }
        }

        // Ordenação eficiente usando qsort
        qsort(grupoFunc, idx, sizeof(TFunc *), compararTFunc);

        // Escreve as partições intercadas em arquivos
        char nomeParticao[100];
        snprintf(nomeParticao, sizeof(nomeParticao), "%s%d.dat", nomeDaParticao, qtdParticoes + 1 + grupo / 4);
        FILE *filePartition = fopen(nomeParticao, "wb+");
        if (!filePartition) {
            printf("Erro ao criar a particao %s\n", nomeParticao);
            return -1;
        }

        for (int i = 0; i < idx; i++) {
            fwrite(grupoFunc[i], sizeof(TFunc), 1, filePartition);
            free(grupoFunc[i]);
        }

        fclose(filePartition);
    }

    for (int i = 0; i < qtdParticoes; i++) {
        if (particoes[i]) fclose(particoes[i]);
        free(funcVet[i]);
    }

    return 0;
}


// Função para realizar a seleção com substituição como método de ordenação
int TFuncSelecaoComSubstituicao(FILE *file, const char *table_name) {
    fflush(file);
    int ok = clearFolder("data/particions");
    if (ok == -1) return ok;

    int numeroDeParticoes = 0;
    unsigned long menorId;

    int M = 6;
    TFunc func_vet[6]; // Array de M registros
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
    if (header.tables[index].size != sizeof(TFunc)) {
        perror("Tamanho do membro incompatível com a tabela");
        return -1;
    }

    // Ler M registros do arquivo para a memória
    fseek(file, header.tables[index].start_offset, SEEK_SET);
    for (int i = 0; i < M; ++i) {
        if (fread(&func_vet[i], sizeof(TFunc), 1, file) != 1) {
            auxVetPks[i] = ULONG_MAX;  // Marcar posição como vazia se não for possível ler
        } else {
            auxVetPks[i] = func_vet[i].pk; // Guardar o ID do registro
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
            fwrite(&func_vet[posicaoMenorId], sizeof(TFunc), 1, filePartition);

            // Ler o próximo registro do arquivo
            TFunc novoFunc;
            if (fread(&novoFunc, sizeof(TFunc), 1, file) == 1) {
                // Substituir o registro no buffer
                func_vet[posicaoMenorId] = novoFunc;
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
    TFunc exerc;
    FILE *aux;
} TVet;


// Função auxiliar para construir a árvore de vencedores
void TFuncConstruirArvoreVencedores(TVet *v, int *arvore, int num_particoes) {
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
void TFuncAtualizarArvoreVencedores(TVet *v, int *arvore, int num_particoes, int pos) {
    pos += num_particoes;
    while (pos > 1) {
        pos /= 2;
        int esquerda = arvore[2 * pos];
        int direita = arvore[2 * pos + 1];
        arvore[pos] = (v[esquerda].exerc.pk <= v[direita].exerc.pk) ? esquerda : direita;
    }
}

int TFuncIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    TVet *v = (TVet *) malloc(num_particoes * sizeof(TVet));
    if (v == NULL) {
        perror("Erro ao alocar memória para vetores de partições");
        return -1;
    }
    // O filho esquerdo está em 2 * i.
    // O filho direito está em 2 * i + 1.
    // O pai está em i / 2.
    int *arvore = (int *) malloc((2 * num_particoes) * sizeof(int));
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
            fread(&v[i].exerc, sizeof(TFunc), 1, v[i].aux);
            if (v[i].exerc.pk == 0) {
                v[i].exerc.pk = ULONG_MAX;
            }
        } else {
            v[i].exerc.pk = ULONG_MAX;  // Marca a partição como finalizada
        }
    }

    // Constroi a árvore de vencedores inicialmente
    TFuncConstruirArvoreVencedores(v, arvore, num_particoes);

    // Reinicializa o arquivo de saída
    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    while (!fim) {
        int vencedor = arvore[1]; // A raiz da árvore de vencedores contém o índice do menor elemento

        if (v[vencedor].exerc.pk == ULONG_MAX) {
            fim = 1;  // Todas as partições acabaram
        } else {
            fwrite(&v[vencedor].exerc, sizeof(TFunc), 1, file);

            // Carrega o próximo elemento da partição vencedora
            if (fread(&v[vencedor].exerc, sizeof(TFunc), 1, v[vencedor].aux) != 1) {
                v[vencedor].exerc.pk = ULONG_MAX; // Marca a partição como finalizada
            }

            // Atualiza a árvore de vencedores com o novo elemento da partição vencedora
            TFuncAtualizarArvoreVencedores(v, arvore, num_particoes, vencedor);
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


/* void unirParticoesOrdenadas(DatabaseHeader *header, char nomeDaParticao[], int qtdParticoes) {
    FILE *saidaFinal = fopen("saidaFinalOrdenada.dat", "wb+");
    if (!saidaFinal) {
        perror("Erro ao criar o arquivo de saída final ordenada.");
        exit(1);
    }

    fwrite(header, sizeof(DatabaseHeader), 1, saidaFinal);

    TFunc *registros[qtdParticoes];
    FILE *particoes[qtdParticoes];
    char nomeArqParticao[100];

    for (int i = 0; i < qtdParticoes; i++) {
        snprintf(nomeArqParticao, sizeof(nomeArqParticao), "%s%d.dat", nomeDaParticao, i);
        particoes[i] = fopen(nomeArqParticao, "rb");
        if (!particoes[i]) {
            printf("Erro ao abrir o arquivo da partição %s\n", nomeArqParticao);
            exit(1);
        }

        registros[i] = (TFunc *)malloc(sizeof(TFunc));
        if (!registros[i] || fread(registros[i], sizeof(TFunc), 1, particoes[i]) != 1) {
            registros[i] = NULL;
        }
    }

    while (1) {
        int idxMenor = -1;
        unsigned long menorPk = ULONG_MAX;

        for (int i = 0; i < qtdParticoes; i++) {
            if (registros[i] && registros[i]->pk < menorPk) {
                menorPk = registros[i]->pk;
                idxMenor = i;
            }
        }

        if (idxMenor == -1) break;

        fwrite(registros[idxMenor], sizeof(TFunc), 1, saidaFinal);
        free(registros[idxMenor]);

        registros[idxMenor] = (TFunc *)malloc(sizeof(TFunc));
        if (!registros[idxMenor] || fread(registros[idxMenor], sizeof(TFunc), 1, particoes[idxMenor]) != 1) {
            free(registros[idxMenor]);
            registros[idxMenor] = NULL;
            fclose(particoes[idxMenor]);
        }
    }

    fclose(saidaFinal);
} */



// ###############################################

int TFunc_HashAdd(FILE* file, const char* table_name, TFunc func) {
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    unsigned long pk = func.pk; // Acesso direto ao campo pk
    if (pk == 0) {
        pk = header.tables[index].next_pk++;
        func.pk = pk; // Acesso direto para definir o pk
    }

    size_t hash_index = hash(pk, header.tables[index].size);
    printf("Tentando escrever o índice: %zu\n", hash_index);

    TFunc buffer;
    size_t previous_index = hash_index;

    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(&buffer, sizeof(TFunc), 1, file);

        if (read_result != 1) {
            // Posição vazia encontrada, adicione o novo registro
            buffer.next_pk = 0; // Este será o último registro
            fseek(file, previous_index, SEEK_SET);
            fwrite(&buffer, sizeof(TFunc), 1, file); // Atualiza o registro anterior
            fseek(file, hash_index, SEEK_SET);
            fwrite(&func, sizeof(TFunc), 1, file); // Escreve o novo registro
            break;
        }

        if (buffer.pk == pk) { // Verifica se a PK já existe
            perror("PK fornecida já existe");
            return ERR_REGISTER_WRITE_FAILED;
        }

        previous_index = hash_index; // Atualiza o índice anterior
        hash_index = buffer.next_pk; // Acesso direto ao campo next_pk
        if (hash_index == 0) {
            fseek(file, 0, SEEK_END);
            size_t end = ftell(file); // Obtenha a posição final

            buffer.next_pk = end; // Atualiza o ponteiro do registro anterior
            fseek(file, previous_index, SEEK_SET);
            fwrite(&buffer, sizeof(TFunc), 1, file);

            func.next_pk = 0; // Fim da lista
            fseek(file, end, SEEK_SET);
            fwrite(&func, sizeof(TFunc), 1, file); // Escreve o novo registro no final
            break;
        }
    }

    // Atualiza o cabeçalho conforme necessário...
    header.tables[index].qtd++; // Atualiza a contagem de registros

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);

    fflush(file);
    return DB_OK;
}
