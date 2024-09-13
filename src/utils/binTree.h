# ifndef BINTREE_H
# define BINTREE_H

typedef struct __node_t {
    struct __node_t *left;
    struct __node_t *right;
    void *data;
} node_t;

// Função de comparação para os dados
typedef int (*compare_func)(const void *a, const void *b);

node_t* create_node(void *data);

node_t* insert_node(node_t *root, void *data, compare_func cmp);

node_t* find_min(node_t *root);

node_t* select_winner(node_t *root, int (*cmp)(const void*, const void*)) ;

void free_tree(node_t *root);

int binTreeTeste();

#endif