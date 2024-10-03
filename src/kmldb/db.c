#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXT ".dat"
#define B_FILENAME 256

FILE* dbInit(const char* filename) {
    char full_filename[B_FILENAME];
    size_t len = strlen(filename);
    // Verifica se o nome do arquivo já possui a extensão ".dat"
    if (len < strlen(EXT) || strcmp(filename + len - strlen(EXT), EXT) != 0) {
        _snprintf_s(full_filename, sizeof(full_filename), _TRUNCATE, "%s%s", filename, EXT);
    } else {
        _snprintf_s(full_filename, sizeof(full_filename), _TRUNCATE, "%s", filename);
    }
    // Tenta abrir o arquivo em modo leitura e escrita binária
    FILE* file = fopen(full_filename, "rb+");
    if (file == NULL) {
        // Se o arquivo não existe, cria um novo banco de dados
        file = fopen(full_filename, "wb+");
        if (file == NULL) {
            perror("Erro ao criar o banco de dados");
            exit(EXIT_FAILURE);
        }
        DatabaseHeader header = {0};
        fwrite(&header, sizeof(DatabaseHeader), 1, file);
        fflush(file);
    } else {
        fseek(file, 0, SEEK_SET); // Garante que o cursor está no início do arquivo
    }
    return file;
}

int dbClose(FILE* file) {
    if (file != NULL) {
        if (fclose(file) != 0) {
            perror("Erro ao fechar o arquivo");
            return ERR_DB_CLOSE_FAILED;
        }
        printf("Database encerrado com sucesso\n");
        return DB_OK;
    }
    return ERR_DB_CLOSE_FAILED;
}
// Definindo códigos de cores ANSI
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

void dbWelcome() {
    printf("\n\n\n");

    // Texto colorido usando códigos ANSI
    printf(BOLD MAGENTA " $$\\   $$\\ $$\\      $$\\ $$\\       $$$$$$$\\  $$$$$$$\\                            \n");
    printf(" $$ | $$  |$$$\\     $$ |$$ |      $$  __$$\\ $$  __$$\\                           \n");
    printf(" $$ |$$  / $$$\\$$\\ $$$ |$$ |      $$ |  $$ |$$ |  $$ |                          \n");
    printf(" $$$$$  /  $$\\$$\\$$ $$ |$$ |      $$ |  $$ |$$$$$$$\\ |                          \n");
    printf(" $$  $$<   $$ \\$$$  $$ |$$ |      $$ |  $$ |$$  __$$\\                            \n");
    printf(" $$ |\\$$\\  $$ |\\$  /$$ |$$ |      $$ |  $$ |$$ |  $$ |                          \n");
    printf(" $$ | \\$$\\ $$ | \\_/ $$ |$$$$$$$$\\ $$$$$$$  |$$$$$$$  |                          \n");
    printf(" \\__|  \\__|\\__|     \\__|\\________|\\_______/ \\_______/                           \n");
    
    printf("\n\n\n");
    
    printf(BOLD GREEN " /$$$$$$\\   $$$$$$\\ $$$$$$$$\\  $$$$$$\\  $$$$$$$\\    $$$$\\    $$$$$\\   $$$$$$$$\\ \n");
    printf(" $$  __$$\\ $$  __$$\\\\__$$  __|$$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  _____|\n");
    printf(" $$ |  $$ |$$ /  $$ |  $$ |   $$ /  $$ |$$ |  $$ |$$ /  $$ |$$ /  \\__|$$ |      \n");
    printf(" $$ |  $$ |$$$$$$$$ |  $$ |   $$$$$$$$ |$$$$$$$\\ |$$$$$$$$ |\\$$$$$$\\  $$$$$\\    \n");
    printf(" $$ |  $$ |$$  __$$ |  $$ |   $$  __$$ |$$  __$$\\ $$  __$$ | \\____$$\\ $$  __|   \n");
    printf(" $$ |  $$ |$$ |  $$ |  $$ |   $$ |  $$ |$$ |  $$ |$$ |  $$ |$$\\   $$ |$$ |      \n");
    printf(" $$$$$$$  |$$ |  $$ |  $$ |   $$ |  $$ |$$$$$$$  |$$ |  $$ |\\$$$$$$  |$$$$$$$$\\ \n");
    printf(" \\_______/ \\__|  \\__|  \\__|   \\__|  \\__|\\_______/ \\__|  \\__| \\______/ \\________|\n");
    
    printf(RESET "\n\n\n");
}

int dbFindTable(FILE* file, const char* table_name) {
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    for (int i = 0; i < header.table_count; i++) {
        if (strcmp(header.tables[i].table_name, table_name) == 0) {
            return i; // Retorna o índice da tabela encontrada
        }
    }
    return -1; // Tabela não encontrada
}

int dbCreateTable(FILE* file, const char* table_name, size_t size) {
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    if (header.table_count >= TABLE_MAX_COUNT) {
        perror("Numero maximo de tabelas atingido");
        return ERR_TABLE_MAX_SIZE_EXCEEDED;
    }
    if (dbFindTable(file, table_name) != -1) {
        perror("Tabela ja existe");
        return ERR_TABLE_EXISTS;
    }
    if (strlen(table_name) >= B_TABLE_NAME - 1) {
        perror("Nome da tabela muito grande ou sem terminacao nula");
        return ERR_TABLE_INVALID_NAME;
    }

    TableMeta* meta = &header.tables[header.table_count];
    strncpy_s(meta->table_name, sizeof(meta->table_name), table_name, _TRUNCATE);
    meta->next_pk = 1; // Define o próximo PK
    meta->start_offset = (header.table_count == 0) ? sizeof(DatabaseHeader) : header.tables[header.table_count - 1].end_offset;
    meta->end_offset = meta->start_offset;
    meta->size = size;

    header.table_count++;

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
    return DB_OK;
}

void dbPrintHeader(DatabaseHeader* header) {
    printf("\n# DATABASE HEADER:\n");
    if (header == NULL) {
        perror("Null Header\n");
        return;
    }
    printf("| Table count: %d\n", header->table_count);
    for (int i = 0; i < header->table_count; ++i) {
        printf("| > TABLE %d:\n", i);
        printf("  | Name: %s\n", header->tables[i].table_name);
        printf("  | Next PK: %lu\n", header->tables[i].next_pk);
        printf("  | Start offset: %ld\n", header->tables[i].start_offset);
        printf("  | End offset: %ld\n", header->tables[i].end_offset);
        printf("  | Size: %zu\n", header->tables[i].size);
    }
}

int dbAdd(FILE* file, const char* table_name, void* member, size_t member_size, size_t pk_offset) {
    DatabaseHeader header;
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
    if (header.tables[index].size != member_size) {
        perror("Tamanho do membro incompatível com a tabela");
        return ERR_REGISTER_INCOMPATIBLE_SIZE;
    }

    unsigned long *pk = (unsigned long *)((char *)member + pk_offset);
    if (*pk == 0) {
        // Atribuir uma nova PK se a PK fornecida for 0
        *pk = header.tables[index].next_pk;
        header.tables[index].next_pk++;
    } else {
        // Verificar se a PK fornecida já existe
        size_t table_offset = header.tables[index].start_offset;
        size_t size = header.tables[index].size;
        while (table_offset < header.tables[index].end_offset) {
            fseek(file, table_offset, SEEK_SET);
            unsigned long existing_pk;
            if (fread(&existing_pk, sizeof(unsigned long), 1, file) != 1) {
                perror("Erro ao ler PK existente");
                return ERR_REGISTER_READ_FAILED;
            }
            if (existing_pk == *pk) {
                perror("PK fornecida já existe");
                return ERR_REGISTER_WRITE_FAILED;
            }
            table_offset += size;
            // Garantir que o next_pk seja maior que o pk fornecido
            if (*pk >= header.tables[index].next_pk) {
                header.tables[index].next_pk = *pk + 1;
            }
        }
    }

    // Posiciona no final da tabela e escreve o membro no arquivo
    if (fseek(file, header.tables[index].end_offset, SEEK_SET) != 0) {
        perror("Erro ao posicionar o ponteiro do arquivo");
        return ERR_REGISTER_READ_FAILED;
    }
    if (fwrite(member, member_size, 1, file) != 1) {
        perror("Erro ao escrever o membro no arquivo");
        return ERR_REGISTER_WRITE_FAILED;
    }

    // Atualiza o end_offset da tabela atual
    size_t size = header.tables[index].size;
    header.tables[index].end_offset += size;

    // Atualiza os offsets das tabelas subsequentes (empurrando para a direita)
    for (int i = index + 1; i < header.table_count; ++i) {
        header.tables[i].start_offset += size;
        header.tables[i].end_offset += size;
    }

    // Reposiciona para escrever o cabeçalho de volta no arquivo
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Erro ao reposicionar o ponteiro do arquivo para o início");
        return ERR_HEADER_READ_FAILED;
    }
    if (fwrite(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao escrever o cabeçalho atualizado no arquivo");
        return ERR_HEADER_WRITE_FAILED;
    }

    fflush(file);
    return DB_OK;
}

int dbReadAll(FILE* file, const char* table_name, void* reg, size_t reg_size, PrintCallback printFunc) {
    DatabaseHeader header;
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

    if (header.tables[index].size != reg_size) {
        perror("Tamanho do registro incompatível com a tabela");
        return ERR_REGISTER_INCOMPATIBLE_SIZE;
    }

    fseek(file, header.tables[index].start_offset, SEEK_SET);
    int count = 0;
    for (count = 0; count < header.tables[index].end_offset / reg_size; count++) {
        if (fread(reg, reg_size, 1, file) != 1) {
            break;
        }
        printFunc(reg);  // Usa a função de callback para imprimir o registro
    }

    return count;
}

int dbReadAllNoHeader(FILE* file, void* member, size_t member_size, PrintCallback printFunc) {
    if (file == NULL) {
        perror("Arquivo inválido");
        return -1;
    }

    if (member == NULL) {
        perror("Ponteiro member inválido");
        return -1;
    }

    fseek(file, 0, SEEK_SET);  // Volta para o início do arquivo
    int i = 0;

    while (fread(member, member_size, 1, file) == 1) {
        printFunc(member);  // Usa a função de callback para imprimir o registro
        i++;
    }

    if (feof(file)) {
        return i;  // Retorna o número de registros lidos
    } else {
        perror("Erro ao ler o arquivo");
        return -1;  // Retorna -1 em caso de erro
    }
}


void dbAddMember2(FILE* file, const char* table_name, void* member, size_t member_size, size_t pk_offset) {
    DatabaseHeader header;
    FILE* temp_file;
    char temp_filename[] = "data/temp_database.dat";
    char buffer[1024];
    
    // Ler o cabeçalho do arquivo original
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return;
    }
    if (pk_offset != 0) {
        perror("A struct deve conter o campo pk no início");
        return;
    }

    // Encontrar a tabela e obter a posição de inserção
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return;
    }

    // Abrir o arquivo auxiliar para escrita
    temp_file = fopen(temp_filename, "wb");
    if (!temp_file) {
        perror("Erro ao criar arquivo auxiliar");
        return;
    }

    size_t insert_pos = header.tables[index].end_offset;
    size_t current_pos = 0;
    fseek(file, current_pos, SEEK_SET);

    // Copiar os dados do arquivo original para o arquivo auxiliar
    while (current_pos < insert_pos) {
        size_t bytes_to_copy = (insert_pos - current_pos > sizeof(buffer)) ? sizeof(buffer) : insert_pos - current_pos;
        size_t bytes_read = fread(buffer, 1, bytes_to_copy, file);
        if (bytes_read != bytes_to_copy && ferror(file)) {
            perror("Erro ao ler do arquivo original");
            fclose(temp_file);
            return;
        }
        fwrite(buffer, 1, bytes_read, temp_file);
        if (ferror(temp_file)) {
            perror("Erro ao escrever no arquivo auxiliar");
            fclose(temp_file);
            return;
        }
        current_pos = ftell(file);
    }

    // Escrever o novo membro no arquivo auxiliar
    fwrite(member, member_size, 1, temp_file);
    if (ferror(temp_file)) {
        perror("Erro ao escrever o novo membro no arquivo auxiliar");
        fclose(temp_file);
        return;
    }

    // Continuar copiando os dados restantes do arquivo original para o arquivo auxiliar
    while (fread(buffer, 1, sizeof(buffer), file) > 0) {
        fwrite(buffer, 1, sizeof(buffer), temp_file);
        if (ferror(temp_file)) {
            perror("Erro ao escrever dados restantes no arquivo auxiliar");
            fclose(temp_file);
            return;
        }
    }

    // Atualiza o end_offset da tabela atual
    size_t size = header.tables[index].size;
    header.tables[index].end_offset += size;
    header.tables[index].qtd += 1;
    // Atualiza os offsets das tabelas subsequentes (empurrando para a direita)
    for (int i = index + 1; i < header.table_count; ++i) {
        header.tables[i].start_offset += size;
        header.tables[i].end_offset += size;
    }
    // Reposiciona para escrever o cabeçalho de volta no arquivo
    if (fseek(temp_file, 0, SEEK_SET) != 0) {
        perror("Erro ao reposicionar o ponteiro do arquivo para o início");
        return;
    }
    if (fwrite(&header, sizeof(DatabaseHeader), 1, temp_file) != 1) {
        perror("Erro ao escrever o cabeçalho atualizado no arquivo");
        return;
    }
    fflush(temp_file);

    // Substituir o arquivo original pelo arquivo auxiliar
    if (remove("data/database.dat") != 0) {
        perror("Erro ao remover o arquivo original");
        return;
    }
    if (rename(temp_filename, "data/database.dat") != 0) {
        perror("Erro ao renomear o arquivo auxiliar");
        return;
    }
    fflush(file);
}

// Um novo dia, uma nova aventura

size_t hash(unsigned long pk, size_t tamanho_registro) {
    return (pk % 31) * tamanho_registro + sizeof(DatabaseHeader); // o 31 é totalmente aleatório.
}

unsigned long get_pk(void* member, size_t pk_offset) {
    return *(unsigned long *)((char *)member + pk_offset);
}

size_t get_next_pk(void* member, size_t next_pk_offset) {
    return *(size_t *)((char *)member + next_pk_offset);
}

void set_pk(void* member, size_t pk_offset, unsigned long pk) {
    *(unsigned long *)((char *)member + pk_offset) = pk;
}

void set_next_pk(void* member, size_t next_pk_offset, size_t next_pk) {
    *(size_t *)((char *)member + next_pk_offset) = next_pk;
}

void* generic_read(FILE* file, size_t offset, size_t member_size) {
    void* data = malloc(member_size);
    if (data == NULL) {
        perror("Erro ao alocar memória");
        return NULL; // Retorna NULL em caso de erro
    }
    fseek(file, offset, SEEK_SET);
    fread(data, member_size, 1, file);
    return data;
}


int generic_write(FILE* file, void* data, size_t size) {
    return fwrite(data, size, 1, file);
}


int dbHashAdd(FILE* file, const char* table_name, void* reg, size_t reg_size,
              size_t pk_offset, size_t next_pk_offset,
              void* (*generic_read)(FILE*, size_t, size_t),
              int (*generic_write)(FILE*, void*, size_t)) {

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    unsigned long pk = *(unsigned long*)((char*)reg + pk_offset);
    if (pk == 0) {
        pk = header.tables[index].next_pk++;
        *(unsigned long*)((char*)reg + pk_offset) = pk; // Define o PK
    }

    size_t hash_index = hash(pk, header.tables[index].size);

    void* buffer = malloc(reg_size);
    size_t previous_index = hash_index;

    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            // Posição vazia encontrada, adicione o novo registro
            *(size_t*)((char*)buffer + next_pk_offset) = 0; // next_pk = 0
            fseek(file, previous_index, SEEK_SET);
            generic_write(file, buffer, reg_size); // Atualiza o registro anterior
            fseek(file, hash_index, SEEK_SET);
            generic_write(file, reg, reg_size); // Escreve o novo registro
            free(buffer);
            break;
        }

        unsigned long existing_pk = *(unsigned long*)((char*)buffer + pk_offset);
        if (existing_pk == pk) {
            perror("PK fornecida já existe");
            free(buffer);
            return ERR_REGISTER_WRITE_FAILED;
        }

        // Colisão
        printf("Colisão encontrada na posição: %zu (PK existente: %lu). Verificando próximo índice...\n", hash_index, existing_pk);
        previous_index = hash_index;
        hash_index = *(size_t*)((char*)buffer + next_pk_offset);

        // Verifique o próximo índice
        printf("Próximo índice a verificar: %zu\n", hash_index);
        if (hash_index == 0) {

            fseek(file, 0, SEEK_END);
            size_t end = ftell(file); // Obtenha a posição final

            // Atualiza o ponteiro do registro anterior
            *(size_t*)((char*)buffer + next_pk_offset) = end;
            fseek(file, previous_index, SEEK_SET);
            generic_write(file, buffer, reg_size);

            *(size_t*)((char*)reg + next_pk_offset) = 0; // 0 demarca Fim da lista
            fseek(file, end, SEEK_SET);
            generic_write(file, reg, reg_size); // Escreve o novo registro no final
            free(buffer);
            printf("Escreveu no final: %zu\n", end);

            printf("Próximo índice a armazenar (buffer): %zu\n", *(size_t*)((char*)buffer + next_pk_offset));
            printf("Próximo índice a armazenar (reg): %zu\n", *(size_t*)((char*)reg + next_pk_offset));

            break;
        }
    }

    printf("Escreveu no índice: %zu\n\n", hash_index);

    // Atualiza o cabeçalho
    header.tables[index].qtd++;
    long position = ftell(file);
    if (position >= 0) {
        header.tables[index].end_offset = ((unsigned long)position > header.tables[index].end_offset)
        ? (unsigned long)position : header.tables[index].end_offset;
    }

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);

    fflush(file);
    return DB_OK;
}

int dbHashRead(FILE* file, const char* table_name, void* reg, size_t reg_size,
               size_t pk_offset, size_t next_pk_offset,
               void* (*generic_read)(FILE*, size_t, size_t),
               int (*generic_write)(FILE*, void*, size_t)) {

    // Ler o cabeçalho do banco de dados
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    // Encontrar a tabela pelo nome
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    // Obter a chave primária (PK) do registro fornecido
    unsigned long pk = *(unsigned long*)((char*)reg + pk_offset);
    if (pk == 0) {
        perror("PK não pode ser 0 para a leitura");
        return ERR_REGISTER_READ_FAILED;
    }

    // Calcular o índice hash com base na PK
    size_t hash_index = hash(pk, header.tables[index].size);
    printf("Tentando ler o índice: %zu\n", hash_index);

    // Buffer para armazenar os dados lidos do arquivo
    void* buffer = malloc(reg_size);
    if (buffer == NULL) {
        perror("Erro ao alocar memória");
        return ERR_REGISTER_ALLOC_FAILED;
    }

    // Laço para procurar o registro pelo PK, tratando colisões
    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            // Registro não encontrado
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }

        // Verificar se o PK do registro lido corresponde ao PK solicitado
        unsigned long existing_pk = *(unsigned long*)((char*)buffer + pk_offset);
        if (existing_pk == pk) {
            // Registro encontrado, copiar os dados para o registro passado
            memcpy(reg, buffer, reg_size);
            free(buffer);
            return DB_OK;
        }

        // Verificar se há colisão e mover para o próximo registro na lista
        hash_index = *(size_t*)((char*)buffer + next_pk_offset);
        if (hash_index == 0) {
            // Não há mais registros para verificar, fim da lista
            perror("Registro com PK fornecida não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }
    }

    free(buffer);
    return DB_OK;
}

int dbHashFind(FILE* file, const char* table_name, void* reg, size_t reg_size,
               size_t pk_offset, size_t next_pk_offset, size_t status_offset,
               void* (*generic_read)(FILE*, size_t, size_t),
               int (*generic_write)(FILE*, void*, size_t)) {

    // Ler o cabeçalho do banco de dados
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    // Encontrar a tabela pelo nome
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    // Obter a chave primária (PK) do registro fornecido
    unsigned long pk = *(unsigned long*)((char*)reg + pk_offset);
    if (pk == 0) {
        perror("PK não pode ser 0 para a leitura");
        return ERR_REGISTER_READ_FAILED;
    }

    // Calcular o índice hash com base na PK
    size_t hash_index = hash(pk, header.tables[index].size);

    // Buffer para armazenar os dados lidos do arquivo
    void* buffer = malloc(reg_size);
    if (buffer == NULL) {
        perror("Erro ao alocar memória");
        return ERR_REGISTER_ALLOC_FAILED;
    }

    // Laço para procurar o registro pelo PK, tratando colisões
    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            // Registro não encontrado
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }

        // Verificar se o PK do registro lido corresponde ao PK solicitado
        unsigned long existing_pk = *(unsigned long*)((char*)buffer + pk_offset);
        if (existing_pk == pk) {
            // Verificar se o registro está marcado como deletado
            int status = *(int*)((char*)buffer + status_offset);
            if (status == 1) {  // 1 significa que o registro foi deletado
                perror("Registro está marcado como deletado");
                free(buffer);
                return EXEPTION_REG_DELETED;
            }

            // Registro encontrado e está ativo, copiar os dados para o registro passado
            memcpy(reg, buffer, reg_size);
            free(buffer);
            return DB_OK;
        }

        // Verificar se há colisão e mover para o próximo registro na lista
        hash_index = *(size_t*)((char*)buffer + next_pk_offset);
        if (hash_index == 0) {
            // Não há mais registros para verificar, fim da lista
            perror("Registro com PK fornecida não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }

        // Printar a colisão e a posição
        printf("Colisão encontrada na posição: %zu\n", hash_index);
    }

    free(buffer);
    return DB_OK;
}


int dbHashUpdate(FILE* file, const char* table_name, unsigned long pk,
                 void* new_reg, size_t reg_size, size_t pk_offset,
                 size_t next_pk_offset, size_t status_offset,
                 void* (*generic_read)(FILE*, size_t, size_t),
                 int (*generic_write)(FILE*, void*, size_t)) {

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    size_t hash_index = hash(pk, header.tables[index].size);
    printf("Tentando atualizar no índice: %zu\n", hash_index);

    void* buffer = malloc(reg_size);
    size_t previous_index = hash_index;

    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }

        unsigned long existing_pk = *(unsigned long*)((char*)buffer + pk_offset);

        // Verifica se a PK bate
        if (existing_pk == pk) {
            // Verifica se o registro está marcado como deletado
            int status = *(int*)((char*)buffer + status_offset);
            if (status == 1) {
                perror("Registro marcado como deletado, não pode ser atualizado");
                free(buffer);
                return ERR_REGISTER_UPDATE_FAILED;
            }

            // Atualiza o registro com os novos dados
            fseek(file, hash_index, SEEK_SET);
            generic_write(file, new_reg, reg_size); // Sobrescreve o registro existente com os novos dados
            free(buffer);
            break;
        }

        // Se o PK não bater, siga para o próximo registro na lista
        previous_index = hash_index;
        hash_index = *(size_t*)((char*)buffer + next_pk_offset);
        if (hash_index == 0) {
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }
    }

    fflush(file);
    return DB_OK;
}



int dbHashDelete(FILE* file, const char* table_name, unsigned long pk,
                 size_t reg_size, size_t pk_offset, size_t next_pk_offset,
                 size_t status_offset,
                 void* (*generic_read)(FILE*, size_t, size_t),
                 int (*generic_write)(FILE*, void*, size_t)) {

    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return ERR_TABLE_NOT_FOUND;
    }

    size_t hash_index = hash(pk, header.tables[index].size);
    printf("Tentando deletar no índice: %zu\n", hash_index);

    void* buffer = malloc(reg_size);
    size_t previous_index = hash_index;

    while (1) {
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }

        unsigned long existing_pk = *(unsigned long*)((char*)buffer + pk_offset);

        if (existing_pk == pk) {
            // Encontrou o registro, então marca como deletado
            *(int*)((char*)buffer + status_offset) = 1; // Marca o status como 1 (deletado)

            fseek(file, hash_index, SEEK_SET);
            generic_write(file, buffer, reg_size); // Atualiza o registro no arquivo
            free(buffer);
            break;
        }

        // Se o PK não bater, siga para o próximo registro na lista
        previous_index = hash_index;
        hash_index = *(size_t*)((char*)buffer + next_pk_offset);
        if (hash_index == 0) {
            perror("Registro não encontrado");
            free(buffer);
            return ERR_REGISTER_NOT_FOUND;
        }
    }

    // Atualiza a quantidade de registros se necessário
    header.tables[index].qtd--;

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);

    fflush(file);
    return DB_OK;
}

void dbPrintTable(FILE* file, const char* table_name, size_t reg_size,
                  size_t pk_offset, size_t next_pk_offset,
                  size_t status_offset,
                  void (*print_func)(void*)) {
    // Ler o cabeçalho do banco de dados
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    // Encontrar a tabela pelo nome
    int index = dbFindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return;
    }

    // Percorrer todos os registros da tabela
    int count = 0;
    size_t hash_index = sizeof(DatabaseHeader);  // inicio da tabela
    while (hash_index < header.tables[index].end_offset) {
        void* buffer = malloc(reg_size);
        printf("Tentando ler no índice: %zu\n", hash_index);

        if (buffer == NULL) {
            perror("Erro ao alocar memória");
            return;
        }

        // Ler o registro no índice atual
        fseek(file, hash_index, SEEK_SET);
        size_t read_result = fread(buffer, reg_size, 1, file);

        if (read_result != 1) {
            free(buffer);
            break;  // Se não for possível ler, saia do laço
        }

        // Verificar se o registro está ativo
        int status = *(int*)((char*)buffer + status_offset);
        int pk = *(int*)((char*)buffer + pk_offset);
        if (status == 0 && pk != 0) {  // 0 significa que o registro está ativo
            print_func(buffer);  // Chamar a função de impressão
            count++;
        }

        hash_index += reg_size;
        free(buffer);  // Liberar memória alocada
    }
    printf("regisitros lidos: %d\n", count);
}
