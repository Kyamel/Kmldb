#ifndef UTILS_H
#define UTILS_H

void atualizarBarraProgresso(int progresso, int total, double tempoGasto, const char *mensagem);

void cClearInputBuffer();

int evaluateArithmeticExpression(const char *expr);

wchar_t *charToWChar(const char *text);

int clearFolder(const char *pasta);

int classificacaoInterna(FILE *file, const char* table_name);

int intercalacaoBasica(FILE *file, DatabaseHeader header, int num_p, const char* table_name);

int intercalacaoBasica_TTreino(FILE *file, DatabaseHeader header, int num_p);

#endif