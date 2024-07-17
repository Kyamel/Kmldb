#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* DB_Init(const char* filename) {
    FILE* file = fopen(filename, "rb+");
    if (file == NULL) {
        // Se o arquivo não existe, cria um novo banco de dados
        file = fopen(filename, "wb+");
        if (file == NULL) {
            perror("Erro ao criar o banco de dados");
            exit(EXIT_FAILURE);
        }

        // Inicializa o cabeçalho com valores vazios
        DatabaseHeader header = {0};

        fwrite(&header, sizeof(DatabaseHeader), 1, file);
        fflush(file);
        
    } else {
        fseek(file, 0, SEEK_SET); // Garante que estamos no início do arquivo
    }
    return file; // Retorna o ponteiro para o arquivo aberto
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

void DB_CreateTable(FILE* file, const char* table_name) {
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

    TableMeta new_table;
    strcpy(new_table.table_name, table_name);
    new_table.next_id = 1; // Inicializa o ID com 1
    new_table.start_offset = sizeof(DatabaseHeader);
    new_table.end_offset = new_table.start_offset; // Inicialmente sem registros

    header.tables[header.table_count++] = new_table;

    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
}

DatabaseHeader* DB_LoadHeader(FILE* file) {
    DatabaseHeader* header = malloc(sizeof(DatabaseHeader));
    if (header == NULL) {
        perror("Erro ao alocar memória para o cabeçalho");
        return NULL;
    }
    fseek(file, 0, SEEK_SET);
    fread(header, sizeof(DatabaseHeader), 1, file);
    return header;
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
        printf("| Next ID: %d\n", header->tables[i].next_id);
        printf("| Start offset: %ld\n", header->tables[i].start_offset);
        printf("| End offset: %ld\n", header->tables[i].end_offset);
    }
}

void DB_AddMember(FILE* file, const char* table_name, void* member, size_t member_size) {
    // Carrega o cabeçalho do arquivo
    DatabaseHeader header;
    fseek(file, 0, SEEK_SET);
    fread(&header, sizeof(DatabaseHeader), 1, file);

    // Encontra a tabela pelo nome
    int index = -1;
    for (int i = 0; i < header.table_count; ++i) {
        if (strcmp(header.tables[i].table_name, table_name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        perror("Tabela não encontrada");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Posiciona no final da tabela e escreve o membro no arquivo
    fseek(file, header.tables[index].end_offset, SEEK_SET);
    fwrite(member, member_size, 1, file);
    fflush(file);

    // Calcula a nova posição atual
    long new_end_offset = ftell(file);
    long offset_difference = new_end_offset - header.tables[index].end_offset;

    // Atualiza o end_offset da tabela atual
    header.tables[index].end_offset = new_end_offset;

    // Atualiza os offsets das tabelas subsequentes (empurrando para a direita)
    for (int i = index + 1; i < header.table_count; ++i) {
        header.tables[i].start_offset += offset_difference;
        header.tables[i].end_offset += offset_difference;
    }

    // Reposiciona para escrever o cabeçalho de volta no arquivo
    fseek(file, 0, SEEK_SET);
    fwrite(&header, sizeof(DatabaseHeader), 1, file);
    fflush(file);
}
