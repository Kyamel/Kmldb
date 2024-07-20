#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXT ".dat"
#define B_FILENAME 256

FILE* DB_Init(const char* filename) {
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

void DB_Close(FILE* file) {
    if (file != NULL) {
        if (fclose(file) != 0) {
            perror("Erro ao fechar o arquivo");
        }
        printf("Database encerrado com sucesso\n");
    }
}
void DB_Welcome() {
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


int DB_FindTable(FILE* file, const char* table_name) {
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

void DB_CreateTable(FILE* file, const char* table_name, size_t size) {
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    if (header.table_count >= TABLE_MAX_COUNT) {
        perror("Número máximo de tabelas atingido");
        return;
    }
    if (DB_FindTable(file, table_name) != -1) {
        perror("Tabela já existe");
        return;
    }
    if (strlen(table_name) >= B_TABLE_NAME - 1) {
        perror("Nome da tabela muito grande ou sem terminação nula");
        return;
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
}

void DB_PrintHeader(DatabaseHeader* header) {
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

void DB_AddMember(FILE* file, const char* table_name, void* member, size_t member_size, size_t pk_offset) {
    // Carrega o cabeçalho do arquivo
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    if (fread(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        return;
    }
    // Encontra a tabela pelo nome
    int index = DB_FindTable(file, table_name);
    if (index == -1) {
        perror("Tabela não encontrada");
        return;
    }
    // Verifica se o tamanho do membro é compatível com a tabela
    if (header.tables[index].size != member_size) {
        perror("Tamanho do membro incompatível com a tabela");
        return;
    }
    // Verifica se o campo pk está presente na struct membro e também se é o primeiro campo dessa struct
    if (pk_offset != 0) {
        perror("A estrutura deve conter o campo pk como o primeiro");
        return;
    }
    // Atualiza a PK do novo membro
    memcpy_s(member, sizeof(unsigned long), &header.tables[index].next_pk, sizeof(header.tables[index].next_pk));
    header.tables[index].next_pk++;
    // Posiciona no final da tabela e escreve o membro no arquivo
    if (fseek(file, header.tables[index].end_offset, SEEK_SET) != 0) {
        perror("Erro ao posicionar o ponteiro do arquivo");
        return;
    }
    if (fwrite(member, member_size, 1, file) != 1) {
        perror("Erro ao escrever o membro no arquivo");
        return;
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
        return;
    }
    if (fwrite(&header, sizeof(DatabaseHeader), 1, file) != 1) {
        perror("Erro ao escrever o cabeçalho atualizado no arquivo");
        return;
    }
    fflush(file);
    printf("Nova posição final da tabela '%s': %lu\n", table_name, header.tables[index].end_offset);
}
