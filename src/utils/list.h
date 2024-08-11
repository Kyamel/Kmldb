#ifndef LIST_H
#define LIST_H


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
node_t *list_node_init();

/* Get the head of the list. */
node_t *list_get_head(node_t *node);

/* Get the tail of the list. */
node_t *list_get_tail(node_t *node);

/* Free the memory of a node, including its data contents if applicable. */
void list_free_node(node_t *node);

/* Free the memory of an entire list. */
void list_free(node_t *node);

/* Insert a node onto the head of the list. Returns a pointer to the added node. */
node_t *list_add_start(node_t *node, node_t *node_add);

/* Insert a node onto the tail of the list. Returns a pointer to the added node. */
node_t *list_add_end(node_t *node, node_t *node_add);

/* Insert a node before the specified node. Returns a pointer to the added node. */
node_t *list_add_before(node_t *node, node_t *node_add);

/* Insert a node after the specified node. Returns a pointer to the added node. */
node_t *list_add_after(node_t *node, node_t *node_add);

/* Find a node in the list with the specified data. Returns a pointer to the node if found, otherwise NULL. */
node_t *list_find(node_t *node, void *data);

/* Delete a node from the list containing the data. Returns 1 on success, or 0 if not found. */
int list_delete(node_t *node, void *data);

/***************************************************************************/

/***************************************************************************
 * Helper Functions 
 ***************************************************************************/

/* Iterate the list from the head treating each node as a person, printing its contents. */
void print_person_list(node_t *node);

/* Create a person with a name and age, returning a pointer to the person. */
person_t *create_person(char *name, int age);

/* Find a person by name in a list of persons, returning a pointer to the person. */
person_t *find_person(node_t *node, char *name);

/***************************************************************************/

/***************************************************************************
 * Test Functions 
 ***************************************************************************/

/* Function to run all tests */
void test_all();

#endif // LIST_H
