#ifdef _WIN32
#include <windows.h>
#include <minwinbase.h>
#else
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "../kmldb/db.h"
#include "../model/cliente.h"
#include "../model/exercicio.h"
#include "../model/funcionario.h"
#include "../model/treino.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void atualizarBarraProgresso(int progresso, int total, double tempoGasto, const char *mensagem) {
    int bar_width = 50; // Largura da barra de progresso
    int pos = progresso * bar_width / total;

    // Cores ANSI
    const char *cor_vermelho = "\033[31m";   // Vermelho
    const char *cor_laranja = "\033[38;5;208m";  // Laranja
    const char *cor_amarelo = "\033[33m";         // Amarelo
    const char *cor_verde = "\033[32m";          // Verde
    const char *cor_reset = "\033[0m";           // Reset para cor padrão

    // Calcula a cor da barra com base no progresso
    const char *cor_barra;
    int porcentagem = progresso * 100 / total;
    if (porcentagem <= 25) {
        cor_barra = cor_vermelho;
    } else if (porcentagem <= 50) {
        cor_barra = cor_laranja;
    } else if (porcentagem <= 75) {
        cor_barra = cor_amarelo;
    } else {
        cor_barra = cor_verde;
    }

    printf("\r%s [", mensagem);

    for (int j = 0; j < bar_width; ++j) {
        if (j < pos) {
            printf("%s#", cor_barra); // Cor da barra
        } else {
            printf("\033[48;5;232m "); // Espaço para a parte não preenchida da barra
        }
    }

    printf("\033[0m] ");
    printf("\033[38;5;208m%d%% Completo\033[0m - ", porcentagem);
    printf("\033[32mTempo gasto: %.2f segundos\033[0m", tempoGasto); // Verde para o tempo

    fflush(stdout);
}

// Função para limpar o buffer do teclado
void cClearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Função auxiliar para avaliar uma expressão aritmética simples (suporta apenas um operador)
int evaluateArithmeticExpression(const char *expr) {
    char *endPtr;
    long num1 = strtol(expr, &endPtr, 10);

    while (isspace(*endPtr)) endPtr++; // Ignora espaços em branco

    if (*endPtr == '*') {
        return (int)(num1 * strtol(endPtr + 1, NULL, 10));
    } else if (*endPtr == '/') {
        return (int)(num1 / strtol(endPtr + 1, NULL, 10));
    } else if (*endPtr == '+') {
        return (int)(num1 + strtol(endPtr + 1, NULL, 10));
    } else if (*endPtr == '-') {
        return (int)(num1 - strtol(endPtr + 1, NULL, 10));
    }

    // Se não houver operador, retorna o número original
    return (int)num1;
}

// Função para converter string do tipo char para WCHAR
wchar_t *charToWChar(const char *text) {
    size_t size = strlen(text) + 1;
    wchar_t *wText = malloc(sizeof(wchar_t) * size);
    size_t tamanho = mbstowcs(wText, text, size);
    if (tamanho == (size_t)-1) {
        perror("Erro na conversão de string char para WCHAR.\n");
    }
    return wText;
}

// Método para limpar a pasta de partições antes de iniciar
int clearFolder(const char *pasta) {
#ifdef _WIN32
    WIN32_FIND_DATAW findFileData;
    wchar_t *dirPath = charToWChar(pasta);
    wchar_t searchPath[1024];
    swprintf_s(searchPath, 1024, L"%s\\*", dirPath);

    HANDLE hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Erro ao abrir diretório.\n");
        free(dirPath);
        return -1;
    }

    do {
        if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
            wchar_t caminho[1024];
            swprintf_s(caminho, sizeof(caminho) / sizeof(wchar_t), L"%s\\%s", dirPath, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Se for um diretório, chamar limpaPasta recursivamente
                clearFolder((const char *)caminho);  // Recursão
                RemoveDirectoryW(caminho); // Remover o diretório vazio
            } else {
                // Se for um arquivo, remover
                DeleteFileW(caminho);
            }
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
    free(dirPath);
    return 0;
#else
    struct dirent *arquivo;
    DIR *dir = opendir(pasta);

    if (dir == NULL) {
        perror("Erro ao abrir diretório");
        return;
    }

    while ((arquivo = readdir(dir)) != NULL) {
        char caminho[1024];
        snprintf(caminho, sizeof(caminho), "%s/%s", pasta, arquivo->d_name);

        // Ignorar . e .. no diretório
        if (strcmp(arquivo->d_name, ".") != 0 && strcmp(arquivo->d_name, "..") != 0) {
            struct stat st;
            if (stat(caminho, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    // Se for um diretório, chamar limpaPasta recursivamente
                    limpaPasta(caminho);
                    rmdir(caminho); // Remover o diretório vazio
                } else {
                    // Se for um arquivo, remover
                    remove(caminho);
                }
            } else {
                perror("Erro ao obter informações do arquivo");
            }
        }
    }
    closedir(dir);
    return 0;
#endif
    return 0;
}

// #########################
// ALGORITIMOS DE ORDENACAO#
// #########################

// TFunc
unsigned long get_pk_TFunc(void *registro) {
    return ((TFunc*)registro)->pk;
}

void* le_TFunc(FILE *arq) {
    TFunc *func = (TFunc*)malloc(sizeof(TFunc));
    fread(func, sizeof(TFunc), 1, arq);
    return func;
}

void salva_TFunc(void *registro, FILE *arq) {
    fwrite((TFunc*)registro, sizeof(TFunc), 1, arq);
}

void* cria_high_value_TFunc() {
    TFunc *func = (TFunc*)malloc(sizeof(TFunc));
    func->pk = ULONG_MAX;
    return func;
}

// TExerc
unsigned long get_pk_TExerc(void *registro) {
    return ((TExerc*)registro)->pk;
}

void* le_TExerc(FILE *arq) {
    TExerc *exerc = (TExerc*)malloc(sizeof(TExerc));
    fread(exerc, sizeof(TExerc), 1, arq);
    return exerc;
}

void salva_TExerc(void *registro, FILE *arq) {
    fwrite((TExerc*)registro, sizeof(TExerc), 1, arq);
}

void* cria_high_value_TExerc() {
    TExerc *exerc = (TExerc*)malloc(sizeof(TExerc));
    exerc->pk = ULONG_MAX;
    return exerc;
}

// TCliente
unsigned long get_pk_TCliente(void *registro) {
    return ((TCliente*)registro)->pk;
}

void* le_TCliente(FILE *arq) {
    TCliente *cliente = (TCliente*)malloc(sizeof(TCliente));
    fread(cliente, sizeof(TCliente), 1, arq);
    return cliente;
}

void salva_TCliente(void *registro, FILE *arq) {
    fwrite((TCliente*)registro, sizeof(TCliente), 1, arq);
}

void* cria_high_value_TCliente() {
    TCliente *cliente = (TCliente*)malloc(sizeof(TCliente));
    cliente->pk = ULONG_MAX;
    return cliente;
}

// TTreino
unsigned long get_pk_TTreino(void *registro) {
    return ((TTreino*)registro)->pk;
}

void* le_TTreino(FILE *arq) {
    TTreino *treino = (TTreino*)malloc(sizeof(TTreino));
    fread(treino, sizeof(TTreino), 1, arq);
    return treino;
}

void salva_TTreino(void *registro, FILE *arq) {
    fwrite((TTreino*)registro, sizeof(TTreino), 1, arq);
}

void* cria_high_value_TTreino() {
    TTreino *treino = (TTreino*)malloc(sizeof(TTreino));
    treino->pk = ULONG_MAX;
    return treino;
}

typedef struct {
    unsigned long (*get_pk)(void*);
    void* (*le)(FILE*);
    void (*salva)(void*, FILE*);
    void* (*cria_high_value)();
    size_t tamanho_registro;
} Metodos;

Metodos selecionarMetodos(const char* table_name) {
    Metodos m;

    if (strcmp(table_name, "funcionarios") == 0) {
        m.get_pk = get_pk_TFunc;
        m.le = le_TFunc;
        m.salva = salva_TFunc;
        m.cria_high_value = cria_high_value_TFunc;
        m.tamanho_registro = sizeof(TFunc);
    } else if (strcmp(table_name, "exercicios") == 0) {
        m.get_pk = get_pk_TExerc;
        m.le = le_TExerc;
        m.salva = salva_TExerc;
        m.cria_high_value = cria_high_value_TExerc;
        m.tamanho_registro = sizeof(TExerc);
    } else if (strcmp(table_name, "clientes") == 0) {
        m.get_pk = get_pk_TCliente;
        m.le = le_TCliente;
        m.salva = salva_TCliente;
        m.cria_high_value = cria_high_value_TCliente;
        m.tamanho_registro = sizeof(TCliente);
    } else if (strcmp(table_name, "treinos") == 0) {
        m.get_pk = get_pk_TTreino;
        m.le = le_TTreino;
        m.salva = salva_TTreino;
        m.cria_high_value = cria_high_value_TTreino;
        m.tamanho_registro = sizeof(TTreino);
    } else {
        perror("Tabela desconhecida");
        exit(EXIT_FAILURE);
    }

    return m;
}

// Função genérica de classificação interna
int classificacaoInterna(FILE *file, const char* table_name) {
    int M = 100;
    rewind(file); // Posiciona cursor no início do arquivo
    int ok = clearFolder("data/particions");
    if (ok == -1) return ok;

    Metodos m = selecionarMetodos(table_name);

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return ERR_HEADER_NOT_FOUND;
    }

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_REGISTER_NOT_FOUND;
    }

    if (header.tables[index].size != m.tamanho_registro) {
        perror("Tamanho do membro incompatível com a tabela");
        return ERR_REGISTER_INCOMPATIBLE_SIZE;
    }

    // Define os offsets de início e fim
    long start_offset = header.tables[index].start_offset;
    long end_offset = header.tables[index].end_offset;

    fseek(file, start_offset, SEEK_SET);  // Posiciona no início da tabela

    int qtdParticoes = 0;
    char nomeParticao[256];

    while (ftell(file) < end_offset) {  // Continua até atingir o end_offset
        // Lê o arquivo e coloca no vetor
        void *v[M];
        int i = 0;
        while (ftell(file) < end_offset && i < M) {
            v[i] = m.le(file);
            i++;
        }

        // Ajusta o tamanho M caso a leitura tenha terminado antes do vetor
        if (i != M) {
            M = i;
        }

        // Faz a ordenação
        for (int j = 1; j < M; j++) {
            void *f = v[j];
            int k = j - 1;
            while ((k >= 0) && (m.get_pk(v[k]) > m.get_pk(f))) {
                v[k + 1] = v[k];
                k--;
            }
            v[k + 1] = f;
        }

        // Cria arquivo de partição e faz gravação
        sprintf(nomeParticao, "data/particions/particion_%i.dat", qtdParticoes);

        FILE *p;
        if ((p = fopen(nomeParticao, "wb+")) == NULL) {
            printf("Erro ao criar arquivo de saída\n");
        } else {
            for (int j = 0; j < M; j++) {
                fseek(p, j * m.tamanho_registro, SEEK_SET);
                m.salva(v[j], p);
            }
            fclose(p);
            qtdParticoes++;
        }

        for (int j = 0; j < M; j++) {
            free(v[j]);
        }
    }

    return qtdParticoes;
}


// Função genérica de intercalação básica
int intercalacaoBasica(FILE *file, DatabaseHeader header, int num_p, const char* table_name) {
    if(num_p < 0) return -1;

    Metodos m = selecionarMetodos(table_name);

    typedef struct vetor {
        void *reg;  // Registro genérico
        FILE *f;    // Arquivo da partição
    } TVet;

    int fim = 0; // Variável que controla fim do procedimento
    int particao = 0;
    char nome[30]; // Aumentado para garantir tamanho suficiente

    // Cria vetor de partições
    TVet v[num_p];

    // Inicializa todos os registros com NULL e arquivos de partição
    for (int i = 0; i < num_p; i++) {
        sprintf(nome, "data/particions/particion_%i.dat", particao);

        v[i].f = fopen(nome, "rb");
        if (v[i].f != NULL) {
            void *reg = m.le(v[i].f);
            if (reg == NULL) {
                // Arquivo estava vazio, coloca HIGH VALUE nessa posição do vetor
                v[i].reg = m.cria_high_value();
            } else {
                // Conseguiu ler o registro, coloca na posição atual do vetor
                v[i].reg = reg;
            }
        } else {
            fim = 1;
        }

        particao++;
    }

    fflush(file);

    // Preenche o cabeçalho
    if (fwrite(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao escrever o cabeçalho no arquivo de saída");
        for (int i = 0; i < num_p; i++) {
            if (v[i].reg != NULL && v[i].reg != m.cria_high_value()) {
                free(v[i].reg);
            }
            if (v[i].f != NULL) {
                fclose(v[i].f);
            }
        }
        return ERR_HEADER_WRITE_FAILED;
    }

    int i = 0;
    while (!fim) {
        unsigned long menor_pk = ULONG_MAX;
        int pos_menor = -1;

        // Encontra o registro com menor chave no vetor
        for (int j = 0; j < num_p; j++) {
            if (m.get_pk(v[j].reg) < menor_pk) {
                menor_pk = m.get_pk(v[j].reg);
                printf("menor_pk: %lu\n", menor_pk);
                pos_menor = j;
            }
        }

        if (pos_menor == -1) {
            fim = 1; // Nenhum registro válido encontrado, termina o processamento
        } else {
            // Salva o registro no arquivo de saída
            m.salva(v[pos_menor].reg, file);

            // Atualiza posição `pos_menor` do vetor com o próximo registro do arquivo
            void *reg = m.le(v[pos_menor].f);
            if (reg == NULL) {
                v[pos_menor].reg = m.cria_high_value();
            } else {
                v[pos_menor].reg = reg;
            }
        }
    }


    // Fecha arquivos das partições de entrada e libera a memória
    for (int i = 0; i < num_p; i++) {
        if (v[i].f != NULL) {
            fclose(v[i].f);
        }
        //free(v[i].reg);
    }

    return 0; // Sucesso
}

int intercalacaoBasica_TTreino(FILE *file, DatabaseHeader header, int num_p) {
    if (num_p < 0) return -1;

    Metodos m = selecionarMetodos("treinos");

    typedef struct vetor {
        TTreino *reg;  // Registro genérico
        FILE *f;    // Arquivo da partição
    } TVet;

    int fim = 0; // Variável que controla fim do procedimento
    int particao = 0;
    char nome[30]; // Aumentado para garantir tamanho suficiente

    // Cria vetor de partições
    TVet v[num_p];

    // Inicializa todos os registros com NULL e arquivos de partição
    for (int i = 0; i < num_p; i++) {
        sprintf(nome, "data/particions/particion_%i.dat", particao);

        v[i].f = fopen(nome, "rb");
        if (v[i].f != NULL) {
            void *reg = m.le(v[i].f);
            if (reg == NULL) {
                v[i].reg = m.cria_high_value();
            } else {
                v[i].reg = reg;
            }
        } else {
            fim = 1;
        }

        particao++;
    }

    fflush(file);

    // Preenche o cabeçalho
    if (fwrite(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao escrever o cabeçalho no arquivo de saída");
        for (int i = 0; i < num_p; i++) {
            if (v[i].reg != NULL && v[i].reg != m.cria_high_value()) {
                free(v[i].reg);
            }
            if (v[i].f != NULL) {
                fclose(v[i].f);
            }
        }
        return ERR_HEADER_WRITE_FAILED;
    }

    while (!fim) {
        unsigned long menor_pk = ULONG_MAX;
        int pos_menor = -1;

        for (int j = 0; j < num_p; j++) {
            if (m.get_pk(v[j].reg) < menor_pk) {
                menor_pk = m.get_pk(v[j].reg);
                pos_menor = j;
            }
        }

        if (pos_menor == -1) {
            fim = 1;
        } else {
            m.salva(v[pos_menor].reg, file);

            void *reg = m.le(v[pos_menor].f);
            if (reg == NULL) {
                v[pos_menor].reg = m.cria_high_value();
            } else {
                v[pos_menor].reg = reg;
            }
        }
    }

    for (int i = 0; i < num_p; i++) {
        if (v[i].f != NULL) {
            fclose(v[i].f);
        }
    }

    return 0; // Sucesso
}
