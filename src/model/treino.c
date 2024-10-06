#include "treino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../kmldb/db.h"

#include "../utils/util.h"
#include "../utils/list.h"



// Calcula o tamanho da estrutura TTreino
int TTreino_Size() {
    return sizeof(long unsigned) // cpk
         + sizeof(long unsigned) // cpk
         + sizeof(long unsigned) // epk
         + sizeof(char) * BT_NOME // nome
         + sizeof(char) * BT_TIPO // tipo
         + sizeof(int); // duration
}

// Cria uma nova instância de TTreino
TTreino TTreino_New(unsigned long pk, const char* nome, const char* tipo,
    long unsigned cpk, long unsigned epk, int duration, size_t next_pk, int status) {
    TTreino treino = {0};
    if (strlen(nome) >= BT_NOME || strlen(tipo) >= BT_TIPO) {
        perror("TTreino buffer overflow");
        return treino;
    }
    treino.pk = pk;
    treino.next_pk = next_pk;
    treino.status = status;
    strncpy_s(treino.nome, sizeof(treino.nome), nome, _TRUNCATE);
    strncpy_s(treino.tipo, sizeof(treino.tipo), tipo, _TRUNCATE);
    treino.cpk = cpk;
    treino.epk = epk;
    treino.duration = duration;

    return treino;
}


// Função para buscar um treino pelo ID na tabela
TTreino TTreino_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TTreino treino = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return treino;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return treino;
    }
    // binary search
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    long unsigned start = 0;
    long unsigned end = (end_offset - start_offset) / size;

    // Abre o arquivo log.txt para registrar as tentativas
    FILE *log_file = fopen("log.txt", "w");
    if (log_file == NULL) {
        perror("Erro ao abrir o arquivo de log");
        return treino;
    }

    // Registra o tempo de início
    clock_t start_time = clock();

    while (start <= end) {
        long unsigned middle = (start + end) / 2;
        size_t seek = start_offset + (middle * size);
        fseek(file, seek, SEEK_SET);
        fread(&treino, sizeof(TTreino), 1, file);
        // Imprime a tentativa no arquivo de log
        fprintf(log_file, "Tentativa: posicao=%lu, pk=%lu\n", middle, treino.pk);

        if (treino.pk == pk) {
            return treino;
        }
        if (treino.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
     // Registra o tempo final e calcula a duração
    clock_t end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Imprime o tempo total no arquivo de log
    fprintf(log_file, "Tempo total de busca binaria: %.2f segundos\n", total_time);

    return treino;
}

TTreino TTreino_GetByCpkEpk(FILE *file, const char* table_name, long unsigned cpk, long unsigned epk) {
    TTreino treino = {0}; // Inicialize com valores padrão
    DatabaseHeader header;
    
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return treino;
    }
    
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return treino;
    }
    
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    
    // Percorre todos os registros na tabela
    for (long unsigned offset = start_offset; offset < end_offset; offset += size) {
        fseek(file, offset, SEEK_SET);
        TTreino current_treino = TTreino_ReadReg(file);
        
        // Verifica se o registro corresponde à chave composta
        if (current_treino.cpk == cpk && current_treino.epk == epk) {
            return current_treino;
        }
    }
    
    // Retorna um TTreino vazio se não encontrar o registro
    return treino;
}

// Função para buscar um treino pelo cpk (id) do cliente e retornar uma lista de treinos
node_t *TTreino_GetByCpk(FILE *file, const char* table_name, long unsigned cpk) {
    TTreino treino = {0}; // Inicialize com valores padrão
    DatabaseHeader header;
    
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return NULL;
    }
    
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return NULL;
    }
    
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = sizeof(TTreino);
    
    node_t *list = list_node_init(); // Lista para armazenar os registros encontrados
    
    // Percorre todos os registros na tabela
    for (long unsigned offset = sizeof(DatabaseHeader); offset < end_offset; offset += size) {
        fseek(file, offset, SEEK_SET);
        TTreino current_treino; 
        if (fread(&current_treino, sizeof(TTreino), 1, file) != 1) {
            perror("Erro ao ler o registro");
            // Libere a memória da lista antes de retornar
            list_free(list);
            return NULL;
        }
        
        // Verifica se o registro corresponde à chave composta
        if (current_treino.cpk == cpk && current_treino.status != 1) {
            // Cria um novo nó para o registro encontrado
            node_t *node = list_node_init();
            if (node == NULL) {
                perror("Erro ao alocar memória para o nó");
                // Libere a memória da lista antes de retornar
                list_free(list);
                return NULL;
            }
            node->data = malloc(sizeof(TTreino));
            if (node->data == NULL) {
                perror("Erro ao alocar memória para os dados do nó");
                // Libere a memória do nó e da lista antes de retornar
                list_free_node(node);
                list_free(list);
                return NULL;
            }
            memcpy(node->data, &current_treino, sizeof(TTreino));
            // Adiciona o nó à lista
            list_add_end(list, node);
        }
    }
    
    return list;
}

// Função para buscar um treino pelo cpk (id) do cliente
int TTreino_ReadByCpk(FILE *file, const char* table_name, long unsigned cpk) {
    TTreino treino = {0}; // Inicialize com valores padrão
    DatabaseHeader header;
    
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return ERR_HEADER_READ_FAILED;
    }
    
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }
    
    long unsigned start_offset = header.tables[index].start_offset;
    long unsigned end_offset = header.tables[index].end_offset;
    size_t size = header.tables[index].size;
    
    // Percorre todos os registros na tabela
    for (long unsigned offset = start_offset; offset < end_offset; offset += size) {
        fseek(file, offset, SEEK_SET);
        TTreino current_treino; 
        if (fread(&current_treino, sizeof(TTreino), 1, file) != 1) {
            perror("Erro ao ler o registro");
            return ERR_REGISTER_READ_FAILED;
        }        
        // Verifica se o registro corresponde à chave composta
        if (current_treino.cpk == cpk) {
            TTreino_Print(&current_treino);
        }
    }
    return 0;
}

// Função para ler um treino de um arquivo, tá com erro
TTreino TTreino_ReadReg(FILE *file) {
    TTreino treino = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return treino;
    }
    if (fread(&treino.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do pk");
        return treino;
    }
    fread(treino.nome, sizeof(char), sizeof(treino.nome), file);
    fread(treino.tipo, sizeof(char), sizeof(treino.tipo), file);
    if (fread(&treino.cpk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do Cliente cpk");
        return treino;
    }
    if (fread(&treino.epk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do Exercicio epk");
        return treino;
    }
    if (fread(&treino.duration, sizeof(int), 1, file) != 1) {
        perror("Erro na leitura do duration");
        return treino;
    }
    return treino;
}


// Função para imprimir os dados de um treino
void TTreino_Print(TTreino *treino) {
    if (treino == NULL) {
        perror("TTreino nulo\n");
        return;
    }
    printf("# Treino:\n");
    printf("| pk: %lu\n", treino->pk);
    printf("| Cliente cpk: %lu\n", treino->cpk);
    printf("| Exercicio epk: %lu\n", treino->epk);
    printf("| Nome: %s\n", treino->nome);
    printf("| Tipo: %s\n", treino->tipo);
    printf("| Duracao: %d segundos\n", treino->duration);
}


// Função de callback para imprimir MyRecord
void TTreino_PrintGeneric(void* member) {
    TTreino* treino = (TTreino*)member;

    printf("# Treino:\n");
    printf("| pk: %lu\n", treino->pk);
    printf("| Cliente cpk: %lu\n", treino->cpk);
    printf("| Exercicio epk: %lu\n", treino->epk);
    printf("| Nome: %s\n", treino->nome);
    printf("| Tipo: %s\n", treino->tipo);
    printf("| Duracao: %d segundos\n", treino->duration);
}


// Função para realizar a seleção com substituição como método de ordenação
int TTreinoSelecaoComSubstituicaoCpkk(FILE *file, const char *table_name) {
    fflush(file);
    int ok = clearFolder("data/particions");
    if (ok == -1) return ok;

    int M = 10;
    TTreino *registers = malloc(M * sizeof(TTreino));
    TTreino *freeze = malloc(M * sizeof(TTreino));
    if (!registers || !freeze) {
        perror("Erro ao alocar memória");
        free(registers);
        free(freeze);
        return -1;
    }
    
    int numCongelados = 0;
    FILE *aux = NULL;
    int particion = 0;

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        free(registers);
        free(freeze);
        return -1;
    }

    if (header.table_count >= TABLE_MAX_COUNT) {
        perror("Número máximo de tabelas atingido");
        free(registers);
        free(freeze);
        return ERR_TABLE_MAX_SIZE_EXCEEDED;
    }
    if (strlen(table_name) >= B_TABLE_NAME - 1) {
        perror("Nome da tabela muito grande ou sem terminação nula");
        free(registers);
        free(freeze);
        return ERR_TABLE_INVALID_NAME;
    }

    // 1) Ler M registros do arquivo para a memória
    fseek(file, header.tables[0].start_offset, SEEK_SET);
    int num_registros_lidos = 0;
    for (int i = 0; i < M; i++) {
        TTreino treino = {0};
        if (fread(&treino, sizeof(TTreino), 1, file) == 1) {
            registers[num_registros_lidos++] = treino;
        } else {
            break;
        }
    }

    if (num_registros_lidos == 0) {
        free(registers);
        free(freeze);
        return -1;
    }

    // Loop principal para processar o arquivo de entrada
    while (1) {
        int indexMenor = -1;
        for (int j = 0; j < num_registros_lidos; j++) {
            if (registers[j].cpk != -1 && (indexMenor == -1 || registers[j].cpk < registers[indexMenor].cpk)) {
                indexMenor = j;
            }
        }

        if (indexMenor == -1) {
            break;
        }

        if (aux == NULL) {
            char nome_arq[256];
            snprintf(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", particion);
            aux = fopen(nome_arq, "a+b");
            if (aux == NULL) {
                perror("Erro ao criar arquivo de partição");
                free(registers);
                free(freeze);
                return -1; // Retorna erro
            }
        }

        if (fwrite(&registers[indexMenor], sizeof(TTreino), 1, aux) != 1) {
            perror("Erro ao gravar registro na partição");
            fclose(aux);
            free(registers);
            free(freeze);
            return -1; // Retorna erro
        }

        TTreino treino = {0};
        if (fread(&treino, sizeof(TTreino), 1, file) == 1) {
            if (treino.cpk >= registers[indexMenor].cpk) {
                registers[indexMenor] = treino;
            } else {
                if (numCongelados < M) {
                    freeze[numCongelados++] = treino;
                }
            }
        } else {
            registers[indexMenor].cpk = -1;  // Marca o registro como processado
        }

        if (numCongelados == M) {
            if (aux != NULL) {
                fclose(aux);
                aux = NULL;
            }
            memmove(registers, freeze, numCongelados * sizeof(TTreino));
            num_registros_lidos = numCongelados;
            numCongelados = 0;
            particion++;
        }
    }

    if (aux != NULL) {
        fclose(aux);
    }

    free(registers);
    free(freeze);
    return particion + 1;
}

int TTreinoClassificacaoInterna(FILE *file, const char* table_name) {
    rewind(file); // Posiciona o cursor no início do arquivo

    int M = 100;  // Tamanho do buffer
    DatabaseHeader header;
    int qtdParticoes = 0;
    char nomeParticao[256];

    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return ERR_HEADER_NOT_FOUND;
    }

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    if (header.tables[index].size != sizeof(TTreino)) {
        perror("Tamanho do registro incompatível com a tabela");
        return ERR_REGISTER_INCOMPATIBLE_SIZE;
    }

    int start_offset = header.tables[index].start_offset;
    int end_offset = header.tables[index].end_offset;

    fseek(file, start_offset, SEEK_SET);

    while (ftell(file) < end_offset) {
        TTreino *v[M];
        int i = 0;

        while (ftell(file) < end_offset && i < M) {
            v[i] = malloc(sizeof(TTreino));
            if (fread(v[i], sizeof(TTreino), 1, file) == 1) {
                i++;
            } else {
                free(v[i]);
                break;
            }
        }

        if (i != M) {
            M = i;
        }

        // troca a chave seguinte com a anterior caso a anterior seja maior,
        // repete isso para todas as chaves anteriores a analisada até a chave anterior ser menor que a sua respectiva chave seguinte
        // ou até chegar no inicio do vetor: significa que v estava em ordem decrescente
        for (int j = 1; j < M; j++) {
            TTreino *t = v[j];
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
            return ERR_OPEN_FAILED;
        }

        for (int i = 0; i < M; i++) {
            if (fwrite(v[i], sizeof(TTreino), 1, p) != 1) {
                perror("Erro ao gravar registro na partição");
                fclose(p);
                for (int jj = 0; jj < M; jj++) {
                    free(v[jj]);
                }
                return ERR_REGISTER_WRITE_FAILED;
            }
            free(v[i]);
        }

        fclose(p);
        qtdParticoes++;
    }

    return qtdParticoes;
}

int TTreinoIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    typedef struct vetor {
        TTreino treino;
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
            fread(&v[i].treino, sizeof(TTreino), 1, v[i].aux);
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
            if (v[i].treino.pk < menor) {
                menor = v[i].treino.pk;
                pos_menor = i;
            }
        }

        if (menor == ULONG_MAX) {
            fim = 1;
        } else {
            fwrite(&v[pos_menor].treino, sizeof(TTreino), 1, file);
            if (fread(&v[pos_menor].treino, sizeof(TTreino), 1, v[pos_menor].aux) != 1) {
                v[pos_menor].treino.pk = ULONG_MAX;
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
int TTreinoSelecaoComSubstituicao(FILE *file, const char *table_name) {
    fflush(file);
    int ok = clearFolder("data/particions");
    if (ok == -1) return ok;

    int numeroDeParticoes = 0;
    unsigned long menorId;
    int comparacao = 0;

    int M = 6;
    TTreino func_vet[6]; // Array de M registros
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
    if (header.tables[index].size != sizeof(TTreino)) {
        perror("Tamanho do membro incompatível com a tabela");
        return -1;
    }

    // Ler M registros do arquivo para a memória
    fseek(file, header.tables[index].start_offset, SEEK_SET);
    for (int i = 0; i < M; ++i) {
        if (fread(&func_vet[i], sizeof(TTreino), 1, file) != 1) {
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
            fwrite(&func_vet[posicaoMenorId], sizeof(TTreino), 1, filePartition);

            // Ler o próximo registro do arquivo
            TTreino novoFunc;
            if (fread(&novoFunc, sizeof(TTreino), 1, file) == 1) {
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
    TTreino treino;
    FILE *aux;
} TVet;


// Função auxiliar para construir a árvore de vencedores
void TTreinoConstruirArvoreVencedores(TVet *v, int *arvore, int num_particoes) {
    // Inicializa a árvore de vencedores
    for (int i = 0; i < num_particoes; i++) {
        arvore[num_particoes + i] = i; // Folhas são as partições
    }

    // Constroi a árvore de vencedores
    for (int i = num_particoes - 1; i > 0; i--) {
        int esquerda = arvore[2 * i];
        int direita = arvore[2 * i + 1];
        arvore[i] = (v[esquerda].treino.pk <= v[direita].treino.pk) ? esquerda : direita;
    }
}

// Atualiza a árvore de vencedores após uma mudança em uma das folhas
void TTreinoAtualizarArvoreVencedores(TVet *v, int *arvore, int num_particoes, int pos) {
    pos += num_particoes;
    while (pos > 1) {
        pos /= 2;
        int esquerda = arvore[2 * pos];
        int direita = arvore[2 * pos + 1];
        arvore[pos] = (v[esquerda].treino.pk <= v[direita].treino.pk) ? esquerda : direita;
    }
}

int TTreinoIntercalacaoComArvore(FILE *file, DatabaseHeader *header, int num_particoes) {
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
            fread(&v[i].treino, sizeof(TTreino), 1, v[i].aux);
            if (v[i].treino.pk == 0) {
                v[i].treino.pk = ULONG_MAX;
            }
        } else {
            v[i].treino.pk = ULONG_MAX;  // Marca a partição como finalizada
        }
    }

    // Constroi a árvore de vencedores inicialmente
    TTreinoConstruirArvoreVencedores(v, arvore, num_particoes);

    // Reinicializa o arquivo de saída
    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    while (!fim) {
        int vencedor = arvore[1]; // A raiz da árvore de vencedores contém o índice do menor elemento

        if (v[vencedor].treino.pk == ULONG_MAX) {
            fim = 1;  // Todas as partições acabaram
        } else {
            fwrite(&v[vencedor].treino, sizeof(TTreino), 1, file);

            // Carrega o próximo elemento da partição vencedora
            if (fread(&v[vencedor].treino, sizeof(TTreino), 1, v[vencedor].aux) != 1) {
                v[vencedor].treino.pk = ULONG_MAX; // Marca a partição como finalizada
            }

            // Atualiza a árvore de vencedores com o novo elemento da partição vencedora
            TTreinoAtualizarArvoreVencedores(v, arvore, num_particoes, vencedor);
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
