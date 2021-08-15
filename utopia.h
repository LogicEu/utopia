#ifndef UTOPIA_UTILS_H
#define UTOPIA_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*=======================================================

>>>>>>>>>>>>>>  UTOPIA UTILITY LIBRARY   >>>>>>>>>>>>>>>>

ANSI C99 header only utility library implementing simple
generic containers and data structures 

============================================= @eulogic */

typedef struct array_t {
    void* data;
    unsigned int bytes, size, used;
} array_t;

typedef struct queue_t {
    void* data;
    unsigned int size, used, bytes, rear, front;
} queue_t;

typedef struct node_t {
    void* data;
    struct node_t *next, *prev;
} node_t;

typedef struct list_t {
    node_t *head, *tail;
    unsigned int bytes, size;
} list_t;

typedef array_t ustack_t;


/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array(unsigned int size, unsigned int bytes);
array_t* array_new(unsigned int size, unsigned int bytes);
unsigned int array_is_empty(const array_t* array);
unsigned int array_is_full(const array_t* array);
void array_resize(array_t* array, unsigned int size);
void array_cut(array_t* array);
void* array_index(const array_t* array, unsigned int index);
void array_push(array_t* array, void* data);
void array_remove(array_t* array, unsigned int index);
int array_find(const array_t* array, void* data);
void* array_pop(array_t* array);
void array_free(array_t* array);
void array_destroy(array_t* array);

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

node_t* node_new(void* data);
void node_push(node_t* head, void* data);
void* node_pop(node_t* node);
void node_remove(node_t* node);
unsigned int node_count(node_t* head);
node_t* node_find(node_t* head, void* data, unsigned int bytes);
int node_find_index(node_t* first, void* data, unsigned int bytes);
node_t* node_index_forward(node_t* head, unsigned int index);
node_t* node_index_backward(node_t* tail, unsigned int index, unsigned int size);

/********************************
 -> Generic Doubly Linked List <- 
********************************/

list_t list(unsigned int bytes);
list_t* list_new(unsigned int bytes);
unsigned int list_is_empty(const list_t* list);
void* list_index(const list_t* list, unsigned int index);
node_t* list_find_node(const list_t* list, void* data);
node_t* list_index_node(const list_t* list, unsigned int index);
int list_find_index(const list_t* list, void* data);
void list_push(list_t* list, void* data);
void* list_pop(list_t* list);
void* list_pop_node(list_t* list, node_t* node);
void* list_pop_index(list_t* list, unsigned int index);
void list_remove_node(list_t* list, node_t* node);
void list_remove_index(list_t* list, unsigned int index);
void list_free(list_t* list);
void list_destroy(list_t* list);

/*************************
 -> Generic Stack Array <- 
*************************/

ustack_t stack(unsigned int size, unsigned int bytes);
ustack_t* stack_new(unsigned int size, unsigned int bytes);
unsigned int stack_is_empty(const ustack_t* stack);
unsigned int stack_is_full(const ustack_t* stack);
void* stack_index(const ustack_t* stack, unsigned int index);
void* stack_peek(const ustack_t* stack);
void stack_push(ustack_t* stack, void* data);
void* stack_pop(ustack_t* stack);
void stack_resize(ustack_t* stack, unsigned int size);
void stack_cut(ustack_t* stack);
void stack_free(ustack_t* stack);
void stack_destroy(ustack_t* stack);

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue(unsigned int size, unsigned int bytes);
queue_t* queue_new(unsigned int size, unsigned int bytes);
unsigned int queue_is_empty(const queue_t* queue);
unsigned int queue_is_full(const queue_t* queue);
void* queue_index(const queue_t* queue, unsigned int index);
void* queue_peek(const queue_t* queue);
void queue_push(queue_t* queue, void* data);
void* queue_pop(queue_t* queue);
void queue_resize(queue_t* queue, unsigned int size);
void queue_cut(queue_t* queue);
void queue_free(queue_t* queue);
void queue_destroy(queue_t* queue);

#ifdef __cplusplus
}
#endif
#endif
