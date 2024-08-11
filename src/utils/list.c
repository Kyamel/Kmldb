#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/***************************************************************************
 * Helper Types 
 ***************************************************************************/

/* Basic example person data structure. */
typedef struct {
    char *name;
    unsigned int age;
} person_t;

/***************************************************************************/

/***************************************************************************
 * List Types 
 ***************************************************************************/

/* Represents a node in a doubly-linked list. */
typedef struct __node_t {
    struct __node_t *prev;
    struct __node_t *next;
    void *data;
} node_t;

/***************************************************************************/

/***************************************************************************
 * List Functions 
 ***************************************************************************/

/* Initialises a node in a doubly-linked list, returning a pointer to the node. */
node_t *list_node_init() {
    node_t *node = calloc(1, sizeof(node_t));
    return node;
}

/* Get the head of the list. */
node_t *list_get_head(node_t *node) {
    node_t *head = node;
    node_t *prev = head->prev;
    while (prev) {
        head = prev;
        prev = head->prev;
    }
    return head;
}

/* Get the tail of the list. */
node_t *list_get_tail(node_t *node) {
    node_t *tail = node;
    node_t *next = tail->next;
    while (next) {
        tail = next;
        next = tail->next;
    }
    return tail;
}

/* Free the memory of a node, including its data contents if applicable. */
void list_free_node(node_t *node) {
    if (node->data)
        free(node->data);
    free(node);
}

/* Free the memory of an entire list. */
void list_free(node_t *node) {
    node_t *curr = list_get_head(node);
    node_t *next = NULL;
    while (curr) {
        next = curr->next;
        list_free_node(curr);
        curr = next;
    }
}

/* Insert a node on to the head of the list. Returns a pointer to the added
 * node.
 */
node_t *list_add_start(node_t *node, node_t *node_add) {
    node_t *head = list_get_head(node);
    assert (head);
    head->prev = node_add;
    node_add->next = head;
    return node_add;
}

/* Insert a node on to the tail of the list. Returns a pointer to the added
 * node.
 */
node_t *list_add_end(node_t *node, node_t *node_add) {
    node_t *tail = list_get_tail(node);
    assert (tail);
    tail->next = node_add;
    node_add->prev = tail;
    return node_add;
}

/* Insert a node before the specified node. Returns a pointer to the added
 * node.
 */
node_t *list_add_before(node_t *node, node_t *node_add) {
    assert (node && node_add);
    node_t *prev = node->prev;
    if (prev)
        prev->next = node_add;
    node_add->prev = prev;
    node_add->next = node;
    node->prev = node_add;
    return node_add;
}

/* Insert a node after the specified node. Returns a pointer to the added
 * node.
 */
node_t *list_add_after(node_t *node, node_t *node_add) {
    assert (node && node_add);
    node_t *next = node->next;
    if (next)
        next->prev = node_add;
    node_add->prev = node;
    node_add->next = next;
    node->next = node_add;
    return node_add;
}

/* Find a node in the list with the specified data. Returns a pointer to the
 * node if found, otherwise NULL. */
node_t *list_find(node_t *node, void *data) {
    node_t *curr = list_get_head(node);
    while (curr) {
        if (curr->data == data)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

/* Delete a node from the list containing the data. Returns 1 on success,
 * or 0 if not found.
 */
int list_delete(node_t *node, void *data) {
    node_t *curr = list_get_head(node);
    while (curr) {
        if (curr->data) {
            if (curr->data == data) {
                node_t *next = curr->next;
                node_t *prev = curr->prev;
                prev->next = next;
                next->prev = prev;
                list_free_node(curr);
                return 1;
            }
        }
    }
    return 0;
}

/***************************************************************************/

/***************************************************************************
 * Helper Functions 
 ***************************************************************************/

/* Iterate the list from the head treating each node as a person, printing its
 * contents.
 */
void print_person_list(node_t *node) {
    node_t *curr = list_get_head(node);
    while (curr) {
        if (curr->data) {
            person_t *person = (person_t *)curr->data;
            printf("name: %s; age: %d\n", person->name, person->age);
        }
        curr = curr->next;
    }
}

/* Create a person with a name and age, returning a pointer to the person. */
person_t *create_person(char *name, int age) {
    person_t *person = malloc(sizeof(person_t));
    person->name = name;
    person->age = age;
    return person;
}

/* Find a person by name in a list of persons, returning a pointer to the person. */
person_t *find_person(node_t *node, char *name) {
    node_t *curr = list_get_head(node);
    while (curr) {
        person_t *person = (person_t *)curr->data;
        if (strcmp(person->name, name) == 0) 
            return person;
        curr = curr->next;
    }
    return NULL;
}

/***************************************************************************/

/***************************************************************************
 * Test Functions (INCOMPLETE)
 ***************************************************************************/

int i = 0;

node_t *test_list() {
    node_t *head = list_node_init();
    head->data = create_person("John Meikle", 23);

    node_t *node1 = list_node_init();
    node1->data = create_person("Bob Dole", 32);
    list_add_end(head, node1);

    node_t *node2 = list_node_init();
    node2->data = create_person("John Smith", 56);
    list_add_end(head, node2);

    return head;
}

void test_list_node_init() {
    node_t *node = list_node_init();
    assert (node);
    list_free_node(node);
}

void test_list_get_head() {
    node_t *head = test_list();
    assert (list_get_head(head) == head);
    list_free(head);
}

void test_list_get_tail() {
    node_t *head = test_list();
    // ...
    list_free(head);
}

// ...

void test_all() {
    test_list_node_init();
    test_list_get_head();
    test_list_get_tail();
}

/***************************************************************************/

int run_list_test() {
    node_t *head = list_node_init();
    head->data = create_person("John Meikle", 23);

    node_t *node1 = list_node_init();
    node1->data = create_person("Bob Dole", 32);
    list_add_end(head, node1);

    //node_t *new_head = list_node_init();
    //new_head->data = create_person("John Smith", 56);
    //list_add_head(head, new_head);

    print_person_list(head);
    assert(find_person(head, "John Meikle"));
    list_free(head);
    return 0;
}