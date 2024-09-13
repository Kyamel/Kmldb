#include <stdio.h>
#include <stdlib.h>
#include "binTree.h"


// Função para criar um novo nó
node_t* create_node(void *data) {
    node_t *new_node = (node_t*)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->data = data;
    return new_node;
}

// Função para inserir um nó na árvore
node_t* insert_node(node_t *root, void *data, int (*cmp)(const void*, const void*)) {
    if (root == NULL) {
        return create_node(data);
    }

    if (cmp(data, root->data) < 0) {
        root->left = insert_node(root->left, data, cmp);
    } else {
        root->right = insert_node(root->right, data, cmp);
    }

    return root;
}

// Função para encontrar o menor elemento (mínimo)
node_t* find_min(node_t *root) {
    while (root && root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Função para mover o menor elemento para a raiz
node_t* select_winner(node_t *root, int (*cmp)(const void*, const void*)) {
    if (root == NULL) {
        return NULL;
    }

    node_t *min_node = find_min(root);
    if (min_node == root) {
        return root;
    }

    // Encontrar o pai do nó mínimo
    node_t *parent = NULL;
    node_t *current = root;
    while (current && current != min_node) {
        parent = current;
        if (cmp(min_node->data, current->data) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    // Substituir o mínimo na raiz
    if (parent) {
        if (parent->left == min_node) {
            parent->left = min_node->right;
        } else {
            parent->right = min_node->right;
        }

        min_node->right = root->right;
        min_node->left = root->left;
    }

    return min_node;
}

// Função para liberar a árvore
void free_tree(node_t *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

// Função de comparação para inteiros
int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Função de teste
void print_in_order(node_t *root) {
    if (root != NULL) {
        print_in_order(root->left);
        printf("%d ", *(int*)root->data);
        print_in_order(root->right);
    }
}

int binTreeTeste() {
    node_t *root = NULL;
    int values[] = {5, 3, 8, 1, 4, 7, 9};

    // Inserir dados na árvore
    for (unsigned long i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        root = insert_node(root, &values[i], compare_int);
    }

    printf("Árvore em ordem antes da seleção do vencedor, root = %d: ", *(int*)root->data);
    print_in_order(root);
    printf("\n");

    // Selecionar o menor elemento e movê-lo para a raiz
    node_t *new_root = select_winner(root, compare_int);

    printf("Árvore em ordem após a seleção do vencedor, new_root = %d: ", *(int*)new_root->data);
    print_in_order(new_root);
    printf("\n");

    // Libere a memória alocada aqui
    // ...

    return 0;
}