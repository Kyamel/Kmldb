#include "cliente.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../kmldb/db.h"
#include "../utils/log.h"

// Calcula o tamanho da estrutura TCliente
int TCliente_Size() {
    return sizeof(long unsigned) 
         + sizeof(char) * BC_NOME 
         + sizeof(char) * BC_CPF 
         + sizeof(char) * BC_EMAIL 
         + sizeof(char) * BC_TELEFONE 
         + sizeof(char) * BC_EXP_DATE;
}

// Cria uma nova instância de TCliente
TCliente TCliente_New(unsigned long pk, const char* nome, const char* cpf, const char* email, const char* telefone, const char* exp_date) {
    TCliente cliente = {0};

    // Verifica se os tamanhos das strings são válidos
    if (strlen(nome) >= BC_NOME || strlen(cpf) >= BC_CPF || strlen(email) >= BC_EMAIL || strlen(telefone) >= BC_TELEFONE || strlen(exp_date) >= BC_EXP_DATE) {
        perror("TCliente buffer overflow");
        return cliente;
    }

    // Dependendo do ambiente, usa strncpy_s ou strncpy
    #ifdef _MSC_VER
        // Ambiente Microsoft
        cliente.pk = pk;
        strncpy_s(cliente.nome, sizeof(cliente.nome), nome, _TRUNCATE);
        strncpy_s(cliente.cpf, sizeof(cliente.cpf), cpf, _TRUNCATE);
        strncpy_s(cliente.email, sizeof(cliente.email), email, _TRUNCATE);
        strncpy_s(cliente.telefone, sizeof(cliente.telefone), telefone, _TRUNCATE);
        strncpy_s(cliente.exp_date, sizeof(cliente.exp_date), exp_date, _TRUNCATE);
    #else
        // Ambiente Unix/Linux
        cliente.pk = pk;
        strncpy(cliente.nome, nome, BC_NOME - 1);
        cliente.nome[BC_NOME - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.cpf, cpf, BC_CPF - 1);
        cliente.cpf[BC_CPF - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.email, email, BC_EMAIL - 1);
        cliente.email[BC_EMAIL - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.telefone, telefone, BC_TELEFONE - 1);
        cliente.telefone[BC_TELEFONE - 1] = '\0'; // Garantir que a string está terminada

        strncpy(cliente.exp_date, exp_date, BC_EXP_DATE - 1);
        cliente.exp_date[BC_EXP_DATE - 1] = '\0'; // Garantir que a string está terminada
    #endif

    return cliente;
}

// Função para buscar um cliente pelo ID na tabela
TCliente TCliente_GetByPK(FILE *file, const char* table_name, long unsigned pk) {
    TCliente clienteEmpty = {0};
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return clienteEmpty;
    }
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return clienteEmpty;
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
        TCliente cliente;
        fread(&cliente.pk, sizeof(TCliente), 1, file);
        // Registrar tentativa no log
        log_attempt(log_file, middle, cliente.pk);
        if (cliente.pk == pk) {
            // Registrar tentativa no log
            log_attempt(log_file, middle, cliente.pk);
            fclose(log_file);  // Fecha o arquivo de log
            return cliente;
        }
        if (cliente.pk < pk) start = middle + 1;
        else end = middle - 1;
    }
       // Registra o tempo final e calcula a duração
    log_total_time(log_file, start_time);
    fclose(log_file);  // Fecha o arquivo de log

    return clienteEmpty;
}

// Função para ler um cliente de um arquivo
TCliente TCliente_ReadReg(FILE *file) {
    TCliente cliente = {0};  // Inicializa a estrutura com zeros
    if (file == NULL) {
        perror("Arquivo não pode ser NULL");
        return cliente;
    }
    if (fread(&cliente.pk, sizeof(long unsigned), 1, file) != 1) {
        perror("Erro na leitura do PK");
        return cliente;
    }
    fread(cliente.nome, sizeof(char), sizeof(cliente.nome), file);
    fread(cliente.cpf, sizeof(char), sizeof(cliente.cpf), file);
    fread(cliente.email, sizeof(char), sizeof(cliente.email), file);
    fread(cliente.telefone, sizeof(char), sizeof(cliente.telefone), file);
    fread(cliente.exp_date, sizeof(char), sizeof(cliente.exp_date), file);
    return cliente;
}

// Função para imprimir os dados de um cliente
void TCliente_Print(TCliente *cliente) {
    if (cliente == NULL) {
        perror("Cliente nulo\n");
        return;
    }
    printf("# Cliente:\n");
    printf("| PK: %lu\n", cliente->pk);
    printf("| Nome: %s\n", cliente->nome);
    printf("| CPF: %s\n", cliente->cpf);
    printf("| Email: %s\n", cliente->email);
    printf("| Telefone: %s\n", cliente->telefone);
    printf("| Data de Expiracao: %s\n", cliente->exp_date);
}

void TCliente_PrintGeneric(void* member) {
    TCliente* cliente = (TCliente*)member;
    printf("# Cliente:\n");
    printf("| PK: %lu\n", cliente->pk);
    printf("| Nome: %s\n", cliente->nome);
    printf("| CPF: %s\n", cliente->cpf);
    printf("| Email: %s\n", cliente->email);
    printf("| Telefone: %s\n", cliente->telefone);
    printf("| Data de Expiracao: %s\n", cliente->exp_date);
}

int TClienteIntercalacaoBasicaTeste(FILE *file, DatabaseHeader *header, int num_particions) {
    typedef struct vetor {
        TCliente cliente;
        FILE *aux;
    } TVet;

    int fim = 0; // variável que controla o fim do procedimento
    int particao = 0;
    char nome_arq[256]; // Alocar memória suficiente para o nome do arquivo

    // Cria vetor de partições
    TVet *v = malloc(num_particions * sizeof(TVet));
    if (v == NULL) {
        perror("Erro ao alocar memória para vetores de partições");
        return -1; // Retorna código de erro
    }

    // Abre arquivos das partições e lê o primeiro registro
    for (int i = 0; i < num_particions; i++) {
        snprintf(nome_arq, sizeof(nome_arq), "data/particions/particion_%d.dat", i);

        v[i].aux = fopen(nome_arq, "rb");
        if (v[i].aux != NULL) {
            TCliente cliente = {0};
            if (fread(&cliente, sizeof(TCliente), 1, v[i].aux) == 1) {
                v[i].cliente = cliente;
            } else {
                // Arquivo estava vazio
                v[i].cliente = TCliente_New(ULONG_MAX, "", "", "", "", ""); // Supondo que TCliente_New cria um TCliente com PK muito alto
            }
        } else {
            fim = 1; // Marcar o fim se algum arquivo não puder ser aberto
        }
    }

    rewind(file);
    fwrite(header, sizeof(DatabaseHeader), 1, file);

    // Processa os registros
    while (!fim) {
        unsigned long menor = ULONG_MAX;
        int pos_menor = -1;

        // Encontra o cliente com menor chave no vetor
        for (int i = 0; i < num_particions; i++) {
            if (v[i].cliente.pk < menor) {
                menor = v[i].cliente.pk;
                pos_menor = i;
            }
        }

        if (pos_menor == -1) {
            fim = 1; // Termina o processamento se não encontrar um menor
        } else {
            fwrite(&v[pos_menor].cliente, sizeof(TCliente), 1, file);

            TCliente cliente = {0};
            if (fread(&cliente, sizeof(TCliente), 1, v[pos_menor].aux) == 1) {
                v[pos_menor].cliente = cliente;
            } else {
                v[pos_menor].cliente = TCliente_New(ULONG_MAX, "", "", "", "", ""); // Supondo que TCliente_New cria um TCliente com PK muito alto
            }
        }
    }

    // Fecha arquivos das partições de entrada
    for (int i = 0; i < num_particions; i++) {
        if (v[i].aux != NULL) {
            fclose(v[i].aux);
        }
    }

    free(v);
    return 0; // Retorna 0 em caso de sucesso
}

int TClienteClassificacaoInterna(FILE *file, const char* table_name) {
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

    if (header.tables[index].size != sizeof(TCliente)) {
        perror("Tamanho do membro incompatível com a tabela");
        return -1;
    }

    int start_offset = header.tables[index].start_offset;
    int end_offset = header.tables[index].end_offset;
    int nCliente = (end_offset - start_offset) / sizeof(TCliente);

    fseek(file, start_offset, SEEK_SET);

    while (ftell(file) < end_offset) {
        TCliente *v[M];
        int i = 0;

        while (ftell(file) < end_offset && i < M) {
            v[i] = malloc(sizeof(TCliente));
            if (fread(v[i], sizeof(TCliente), 1, file) == 1) {
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
            TCliente *t = v[j];
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
            if (fwrite(v[i], sizeof(TCliente), 1, p) != 1) {
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

int TClienteIntercalacaoBasica(FILE *file, DatabaseHeader *header, int num_particoes) {
    if (num_particoes <= 0) {
        perror("Número de partições inválido");
        return -1;
    }

    typedef struct vetor {
        TCliente cliente;
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
            fread(&v[i].cliente, sizeof(TCliente), 1, v[i].aux);
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
            if (v[i].cliente.pk < menor) {
                menor = v[i].cliente.pk;
                pos_menor = i;
            }
        }

        if (menor == ULONG_MAX) {
            fim = 1;
        } else {
            fwrite(&v[pos_menor].cliente, sizeof(TCliente), 1, file);
            if (fread(&v[pos_menor].cliente, sizeof(TCliente), 1, v[pos_menor].aux) != 1) {
                v[pos_menor].cliente.pk = ULONG_MAX;
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
