#ifndef UTOPIA_UTILS_H
#define UTOPIA_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*=======================================================

>>>>>>>>>>>>>>  UTOPIA UTILITY LIBRARY   >>>>>>>>>>>>>>>>

Simple, easy and generic container implementations in C. 

================================== @Eugenio Arteaga A. */

#include <stddef.h>

typedef struct array_t {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
} array_t;

typedef struct array_t ustack_t;

typedef struct queue_t {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
    size_t rear;
    size_t front;
} queue_t;

typedef struct node_t {
    void* data;
    struct node_t *next;
    struct node_t *prev;
} node_t;

typedef struct list_t {
    node_t *head;
    node_t *tail;
    size_t bytes;
    size_t size;
} list_t;

typedef struct map_t {
    size_t** indexes;
    void* keys;
    void* values;
    size_t key_bytes;
    size_t value_bytes;
    size_t data_capacity;
    size_t element_count;
    size_t (*hash_func)(const void* key);
} map_t;

/*********************************************
 -> Some macros for dangerously fast access <- 
*********************************************/

#define _array_index(array, i) ((char*)(array)->data + (i) * (array)->bytes)
#define _array_pop(array, i) (!array->used) ? NULL : _array_index(array, --array->used)
#define _array_peek(array, i) (!array->used) ? NULL : _array_index(array, array->used - 1)

#define _map_key_at(map, index) ((char*)(map)->keys + (map)->key_bytes * (index))
#define _map_value_at(map, index) ((char*)(map)->values + (map)->value_bytes * (index))

/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array_create(const size_t bytes);
array_t array_reserve(const size_t bytes, const size_t reserve);
array_t array_copy(const array_t* array);
void array_push(array_t* array, const void* data);
void array_remove(array_t* array, const size_t index);
void* array_index(const array_t* array, const size_t index);
void* array_peek(const array_t* array);
void* array_pop(array_t* array);
void array_resize(array_t* array, const size_t size);
void array_cut(array_t* array);
size_t array_find(const array_t* array, const void* data);
size_t array_push_if(array_t* array, const void* data);
void array_set(array_t* array);
void array_clear(array_t* array);
void array_free(array_t* array);

/*************************
 -> Generic Stack Array <- 
*************************/

ustack_t stack_create(const size_t bytes);
ustack_t stack_reserve(const size_t bytes, const size_t reserve);
ustack_t stack_copy(const ustack_t* stack);
void stack_push(ustack_t* stack, const void* data);
void* stack_peek(const ustack_t* stack);
void* stack_pop(ustack_t* stack);
void* stack_index(const ustack_t* stack, const size_t index);
void stack_resize(ustack_t* stack, const size_t size);
void stack_cut(ustack_t* stack);
void stack_clear(ustack_t* stack);
void stack_free(ustack_t* stack);

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue_create(const size_t bytes);
queue_t queue_reserve(const size_t bytes, const size_t reserve);
queue_t queue_copy(const queue_t* queue);
void queue_push(queue_t* queue, const void* data);
void* queue_index(const queue_t* queue, const size_t index);
void* queue_peek(const queue_t* queue);
void* queue_pop(queue_t* queue);
void queue_resize(queue_t* queue, const size_t size);
void queue_cut(queue_t* queue);
void queue_clear(queue_t* stack);
void queue_free(queue_t* queue);

/********************************
 -> Generic Doubly Linked List <- 
********************************/

list_t list_create(const size_t bytes);
int list_is_empty(const list_t* list);
void* list_index(const list_t* list, const size_t index);
node_t* list_find_node(const list_t* list, const void* data);
node_t* list_index_node(const list_t* list, const size_t index);
size_t list_find_index(const list_t* list, const void* data);
void list_push(list_t* list, const void* data);
void* list_pop(list_t* list);
void* list_pop_node(list_t* list, node_t* node);
void* list_pop_index(list_t* list, const size_t index);
void list_remove_node(list_t* list, node_t* node);
void list_remove_index(list_t* list, const size_t index);
void list_free(list_t* list);

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

node_t* node_new(void* data);
void node_push(node_t* head, void* data);
void* node_pop(node_t* node);
void node_remove(node_t* node);
size_t node_count(node_t* head);
node_t* node_find(node_t* head, const void* data, const size_t bytes);
size_t node_find_index(node_t* first, const void* data, const size_t bytes);
node_t* node_index_forward(node_t* head, const size_t index);
node_t* node_index_backward(node_t* tail, const size_t index, const size_t size);

/***********************************
 -> Generic <Key, Value> Hash Map <- 
***********************************/

map_t map_create(const size_t key_size, const size_t value_size);
map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve);
size_t map_search(const map_t* map, const void* key);
size_t* map_search_all(const map_t* map, const void* key);
size_t map_capacity(const map_t* map);
size_t map_element_count(const map_t* map);
void* map_key_at(const map_t* map, const size_t index);
void* map_value_at(const map_t* map, const size_t index);
void map_overload(map_t* map, size_t (*hash_func)(const void* key));
void map_push(map_t* map, const void* key, const void* value);
size_t map_push_if(map_t* map, const void* key, const void* value);
void map_resize(map_t* map, const size_t size);
void map_remove(map_t* map, const void* key);
void map_free(map_t* map);

/*******************************
 -> Hash Functions Prototypes <- 
*******************************/

size_t hash_cstr(const void* key);
size_t hash_uint(const void* key);

#ifdef __cplusplus
}
#endif
#endif
