#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <stdio.h>

// Funções para interação com o menu principal
void cadastrarCliente(FILE *file);

void buscarCliente(FILE *file);

void cadastrarFuncionario(FILE *file);

void buscarFuncionario(FILE *file);

void cadastrarExercicio(FILE *file);

void buscarExercicio(FILE *file);

void cadastrarTreino(FILE *file, FILE *fexerc, FILE *fcli);

void buscarTreino(FILE *file);

int cli_main_menu();

#endif // MAIN_MENU_H
