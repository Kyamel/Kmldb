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
void dbWelcome() {
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("$$\\   $$\\ $$\\      $$\\ $$\\       $$$$$$$\\  $$$$$$$\\                            \n");
    printf("$$ | $$  |$$$\\     $$ |$$ |      $$  __$$\\ $$  __$$\\                           \n");
    printf("$$ |$$  / $$$\\$$\\ $$$ |$$ |      $$ |  $$ |$$ |  $$ |                          \n");
    printf("$$$$$  /  $$\\$$\\$$ $$ |$$ |      $$ |  $$ |$$$$$$$\\ |                          \n");
    printf("$$  $$<   $$ \\$$$  $$ |$$ |      $$ |  $$ |$$  __$$\\                            \n");
    printf("$$ |\\$$\\  $$ |\\$  /$$ |$$ |      $$ |  $$ |$$ |  $$ |                          \n");
    printf("$$ | \\$$\\ $$ | \\_/ $$ |$$$$$$$$\\ $$$$$$$  |$$$$$$$  |                          \n");
    printf("\\__|  \\__|\\__|     \\__|\\________|\\_______/ \\_______/                           \n");
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("$$$$$$$\\   $$$$$$\\ $$$$$$$$\\  $$$$$$\\  $$$$$$$\\    $$$$\\    $$$$$\\   $$$$$$$$\\ \n");
    printf("$$  __$$\\ $$  __$$\\\\__$$  __|$$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\ $$  _____|\n");
    printf("$$ |  $$ |$$ /  $$ |  $$ |   $$ /  $$ |$$ |  $$ |$$ /  $$ |$$ /  \\__|$$ |      \n");
    printf("$$ |  $$ |$$$$$$$$ |  $$ |   $$$$$$$$ |$$$$$$$\\ | $$$$$$$$ |\\$$$$$$\\  $$$$$\\    \n");
    printf("$$ |  $$ |$$  __$$ |  $$ |   $$  __$$ |$$  __$$\\ $$  __$$ | \\____$$\\ $$  __|   \n");
    printf("$$ |  $$ |$$ |  $$ |  $$ |   $$ |  $$ |$$ |  $$ |$$ |  $$ |$$\\   $$ |$$ |      \n");
    printf("$$$$$$$  |$$ |  $$ |  $$ |   $$ |  $$ |$$$$$$$  |$$ |  $$ |\\$$$$$$  |$$$$$$$$\\ \n");
    printf("\\_______/ \\__|  \\__|  \\__|   \\__|  \\__|\\_______/ \\__|  \\__| \\______/ \\________|\n");
    printf("                                                                               \n");
    printf("                                                                               \n");
    printf("                                                                               \n");
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
        perror("Número máximo de tabelas atingido");
        return ERR_TABLE_MAX_SIZE_EXCEEDED;
    }
    if (dbFindTable(file, table_name) != -1) {
        perror("Tabela já existe");
        return ERR_TABLE_EXISTS;
    }
    if (strlen(table_name) >= B_TABLE_NAME - 1) {
        perror("Nome da tabela muito grande ou sem terminação nula");
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
        printf("> TABLE %d:\n", i);
        printf("| Name: %s\n", header->tables[i].table_name);
        printf("| Next PK: %lu\n", header->tables[i].next_pk);
        printf("| Start offset: %ld\n", header->tables[i].start_offset);
        printf("| End offset: %ld\n", header->tables[i].end_offset);
        printf("| Size: %zu\n", header->tables[i].size);
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
        return ERR_REGISTER_NOT_FOUND;
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
