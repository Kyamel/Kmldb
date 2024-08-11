#ifndef UTILS_H
#define UTILS_H



void cClearInputBuffer();

wchar_t *charToWChar(const char *text);

int clearFolder(const char *pasta);

int classificacaoInterna(FILE *file, const char* table_name);

int intercalacaoBasica(FILE *file, DatabaseHeader header, int num_p, const char* table_name);

int intercalacaoBasica_TTreino(FILE *file, DatabaseHeader header, int num_p);

#endif