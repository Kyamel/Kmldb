int registroMemoria (int vet[6]) {

    int cont = 0;

    for (int i = 0; i < 6; ++i) {
        if (vet[i] == 1) {
            cont++;
        }
    }

    if (cont == 6) {
        return 1;
    } else {
        return 0;
    }
}

int tamanhoArquivoParticao(FILE *file) {
    int contSizeFile = 0;
    int bytesAUX = 0;

    while (1) {
        fseek(file, bytesAUX * sizeof(TJogador), SEEK_SET);
        TJogador *j = leJogador(file);
        if (j == NULL) {
            break;
        }
        free(j);
        contSizeFile++;
        bytesAUX++;
    }

    return contSizeFile;
}

void imprimeParticoes(FILE *file, char nomeParticao[]) {
    printf("PARTICAO: %s \n\n", nomeParticao);

    int totalPlayers = tamanhoArquivoParticao(file);

    for (int i = 0; i < totalPlayers; ++i) {
        fseek(file, i * tamanhoRegistroJogador(), SEEK_SET);
        TJogador *j = leJogador(file);

        if (j != NULL) {
            printf("%d ", j->id);
            free(j); // Liberar a mem�ria alocada por le()
        }
    }

    printf("\n");
    printf("\n**\n");

}


int selecaoSubstituicao(FILE *in, char nomeDaParticao[]) {
    int numeroDeParticoes = 0, contadorArquivo = 0, posicao = 6, posicaoMenorId = 0, menorId = 999999, AuxArquivo = 0, posicaoSelecionada = 0;
    int comparacao = 0;
    TJogador jogador[6];
    int auxVetPlayer[6] = {0};

    rewind(in);

    clock_t inicio = clock();

    // Obter o tamanho do arquivo de entrada
    AuxArquivo = tamanhoArquivoParticao(in);

    printf("\nGerando particoes por Selecao com Substituicao...\n\n");

    // Ler os primeiros 6 registros do arquivo e armazená-los na memória (vetor player)
    for (int i = 0; i < 6; ++i) {
        fseek(in, i * tamanhoRegistroJogador(), SEEK_SET);
        TJogador *j = leJogador(in);
        jogador[i] = *j;
        free(j); // Liberar a memória alocada por le()
    }

    // Preencher o vetor auxiliar 'auxVetPlayer' com os IDs dos registros do vetor 'player'
    for (int i = 0; i < 6; ++i) {
        auxVetPlayer[i] = jogador[i].id;
    }

    // Looping da geração de partições
    while (posicao != AuxArquivo) { //Enquanto AuxArquivo for diferente de (6 número de posições)

        // Cria partições
        char nomeParticao[100];
        char str1[100];
        char str2[100] = ".dat";

        itoa(numeroDeParticoes, str1, 10);
        strcat(strcpy(nomeParticao, nomeDaParticao), str1);
        strcat(strcpy(nomeParticao, nomeParticao), str2);

        FILE *filePartition = fopen(nomeParticao, "wb+");

        // Vetor que irá fazer o controle das posições que serão congeladas
        int vetCongelado[6] = {0, 0, 0, 0, 0, 0};

        // Loop interno para selecionar elementos para a partição
        while (posicao != AuxArquivo) {

            menorId = 9999999;

            // Encontrar o menor ID dentre os IDs presentes no vetor auxiliar 'auxVetPlayer'
            for (int i = 0; i < 6; ++i) {
                if (menorId > auxVetPlayer[i] && vetCongelado[i] != 1) {
                    comparacao = comparacao + 1;
                    menorId = auxVetPlayer[i];
                    posicaoMenorId = i;
                }
            }

            // Salvar o registro com menor ID na partição
            salvaJogador(&jogador[posicaoMenorId], filePartition);

            // Ler próximo registro do arquivo de entrada
            fseek(in, posicao * tamanhoRegistroJogador(), SEEK_SET);
            TJogador *j = leJogador(in);

            // Atualiza a posição
            posicao++;

            // Atualizar vetor auxiliar 'auxVetPlayer' e vetor 'player' com o novo registro lido
            auxVetPlayer[posicaoMenorId] = j->id;
            jogador[posicaoMenorId] = *j;

            // Congelar o registro se o ID for menor que o recém gravado na partição de saída
            if (j->id < menorId) {
                comparacao = comparacao + 1;
                vetCongelado[posicaoMenorId] = 1;
            }

            // Verificar se a memória da partição está cheia

            // Se for igual a 1 significa que vetor de congelos está cheio.
            // Volta por passo 2 e descongela
            if (registroMemoria(vetCongelado) == 1) {
                comparacao = comparacao + 1;
                numeroDeParticoes++;
                break;
            }
        }

        // Fechar a partição criada
        fclose(filePartition);

        // Verificar se a última posição foi atingida
        if (posicao >= AuxArquivo) {
            break;
        }
    }

    // Nome da partição final a ser criada
    char nomeParticaoFinal[100];
    char str1[100];
    char str2[100] = ".dat";

    itoa(numeroDeParticoes, str1, 10);
    strcat(strcpy(nomeParticaoFinal, nomeDaParticao), str1);
    strcat(strcpy(nomeParticaoFinal, nomeParticaoFinal), str2);

    // Criar a partição final
    FILE *filePartitionFinal = fopen(nomeParticaoFinal, "ab+");

    // Criar uma lista dinâmica para armazenar os registros restantes
    TJogador *remainingPlayers = malloc((AuxArquivo - posicao) * sizeof(TJogador));
    int remainingCount = 0;

    // Armazenar os registros restantes na lista
    while (posicao < AuxArquivo) {
        fseek(in, posicao * tamanhoRegistroJogador(), SEEK_SET);
        TJogador *j = leJogador(in);
        remainingPlayers[remainingCount++] = *j;
        free(j);
        posicao++;
    }

    // Ordenar os registros restantes usando merge sort
    FILE *tempFile = fopen("tempMerge.dat", "wb+");
    if (tempFile == NULL) {
        perror("Erro ao abrir arquivo temporário para merge sort");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < remainingCount; ++i) {
        salvaJogador(&remainingPlayers[i], tempFile);
    }

    fclose(tempFile);
    tempFile = fopen("tempMerge.dat", "rb+");
    if (tempFile == NULL) {
        perror("Erro ao reabrir arquivo temporário para merge sort");
        exit(EXIT_FAILURE);
    }

    mergeSortDiscoJogador(tempFile);
    fclose(tempFile);

    tempFile = fopen("tempMerge.dat", "rb");
    if (tempFile == NULL) {
        perror("Erro ao reabrir arquivo temporário para leitura");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < remainingCount; ++i) {
        TJogador *j = leJogador(tempFile);
        if (j != NULL) {
            salvaJogador(j, filePartitionFinal);
            free(j);
        }
    }

    fclose(tempFile);
    remove("tempMerge.dat");
    free(remainingPlayers);
    fclose(filePartitionFinal);

    // Looping para imprimir o conteúdo de cada partição gerada
    for (int i = 0; i <= numeroDeParticoes; ++i) {
        itoa(i, str1, 10);
        strcat(strcpy(nomeParticaoFinal, nomeDaParticao), str1);
        strcat(strcpy(nomeParticaoFinal, nomeParticaoFinal), str2);

        FILE *filePartition = fopen(nomeParticaoFinal, "rb+");

        imprimeParticoes(filePartition, nomeParticaoFinal);

        fclose(filePartition);
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    FILE *log_file = fopen("Log_jogadores.txt", "a"); // Modo de adição ao invés de escrita
    if (log_file == NULL) {
        printf("Erro ao abrir o arquivo de log.\n");
        return -1;
    }

    printf("\nTempo de execucao do metodo de selecao: %.2f segundos\n", tempo);
    printf("Numero de comparacoes: %d\n", comparacao);

    fprintf(log_file, "SELECAO POR SUBSTITUICAO");
    fprintf(log_file, "\nTempo de execucao do metodo de selecao: %.2f segundos\n", tempo);
    fprintf(log_file, "Numero de comparacoes: %d\n", comparacao);

    fclose(log_file);

    return numeroDeParticoes;
}