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

#define UTOPIA_HASH_SIZE 32
#define UTOPIA_INDEX_TYPE size_t

typedef UTOPIA_INDEX_TYPE index_t;
typedef index_t* bucket_t;

typedef struct array_t {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
} array_t;

typedef struct queue_t {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t rear;
    size_t front;
} queue_t;

typedef struct table_t {
    index_t* indices;
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
} table_t;

typedef struct string_t {
    char* data;
    size_t capacity;
    size_t size;
} string_t;

typedef struct hash_t {
    bucket_t* indices;
    void* data;
    size_t bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
} hash_t;

typedef struct map_t {
    bucket_t* indices;
    void* keys;
    void* values;
    size_t key_bytes;
    size_t value_bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
} map_t;

typedef struct lnode_t {
    struct lnode_t *next;
    struct lnode_t *prev;
    void* data;
} lnode_t;

typedef struct list_t {
    lnode_t *head;
    lnode_t *tail;
    size_t bytes;
    size_t size;
} list_t;

typedef struct bnode_t {
    struct bnode_t *left;
    struct bnode_t *right;
    void* data;
} bnode_t;

typedef struct btree_t {
    bnode_t* root;
    size_t bytes;
} btree_t;

/*********************************************
 -> Some macros for dangerously fast access <- 
*********************************************/

#define _array_create(bytes) (array_t){NULL, bytes, 0, 0}
#define _array_index(array, i) ((char*)(array)->data + (i) * (array)->bytes)

#define _table_index_at(table, i) ((table)->indices[i + 2])
#define _table_value_at(table, i) (_array_index((table), _table_index_at(table, i)))

#define _map_key_at(map, i) ((char*)(map)->keys + (map)->key_bytes * (i))
#define _map_value_at(map, i) ((char*)(map)->values + (map)->value_bytes * (i))

/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array_create(const size_t bytes);
array_t array_reserve(const size_t bytes, const size_t reserve);
array_t array_wrap(void* data, const size_t bytes, const size_t size);
array_t array_copy(const array_t* array);
array_t array_move(array_t* array);
void array_push(array_t* array, const void* data);
void array_push_vargs(array_t* array, const size_t argc, ...);
void array_push_block(array_t* array, const void* data, const size_t count);
void array_remove(array_t* array, const size_t index);
void array_remove_block(array_t* array, const size_t start, const size_t end);
void* array_data(const array_t* array);
void* array_peek(const array_t* array);
void* array_pop(array_t* array);
void* array_index(const array_t* array, const size_t index);
size_t array_bytes(const array_t* array);
size_t array_size(const array_t* array);
size_t array_capacity(const array_t* array);
size_t array_push_if(array_t* array, const void* data);
size_t array_search(const array_t* array, const void* data);
size_t* array_search_all(const array_t* array, const void* data);
void array_restructure(array_t* array, const size_t bytes);
void array_resize(array_t* array, const size_t size);
void array_cut(array_t* array);
void array_set(array_t* array);
void array_clear(array_t* array);
void array_free(array_t* array);

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue_create(const size_t bytes);
queue_t queue_reserve(const size_t bytes, const size_t reserve);
queue_t queue_copy(const queue_t* queue);
queue_t queue_move(queue_t* queue);
void queue_push(queue_t* queue, const void* data);
void* queue_index(const queue_t* queue, const size_t index);
void* queue_peek(const queue_t* queue);
void* queue_pop(queue_t* queue);
void queue_resize(queue_t* queue, const size_t size);
void queue_cut(queue_t* queue);
size_t queue_bytes(const queue_t* queue);
size_t queue_size(const queue_t* queue);
size_t queue_capacity(const queue_t* queue);
size_t queue_rear(const queue_t* queue);
size_t queue_front(const queue_t* queue);
void queue_clear(queue_t* stack);
void queue_free(queue_t* queue);

/**********************************
 -> Indexed Table Data Structure <- 
**********************************/

table_t table_create(const size_t bytes);
size_t table_push(table_t* table, const void* data);
void table_push_index(table_t* table, const index_t index);
void table_push_data(table_t* table, const void* data);
void table_remove(table_t* table, const index_t index);
table_t table_compress(const array_t* buffer);
array_t table_decompress(const table_t* table);
void* table_values(const table_t* table);
void* table_value_at(const table_t* table, const size_t index);
index_t* table_indices(const table_t* table);
index_t table_index_at(const table_t* table, const size_t index);
size_t table_indices_size(const table_t* table);
size_t table_values_size(const table_t* table);
size_t table_bytes(const table_t* table);
void table_free(table_t* table);

/******************************
 -> Dynamic String Container <- 
******************************/

string_t string_create(const char* data);
string_t string_copy(const string_t* str);
void string_push(string_t* str, const char* buffer);
void string_concat(string_t* str1, const string_t* str2);
size_t string_capacity(const string_t* str);
size_t string_size(const string_t* str);
char* string_data(const string_t* str);
size_t string_search(const string_t* str, const char* search);
size_t* string_search_all(const string_t* str, const char* search);
void string_remove(string_t* str, const char* search);
void string_remove_all(string_t* str, const char* search);
void string_reverse(string_t* str);
void string_clear(string_t* str);
void string_free(string_t* str);

/********************************
 -> Generic Hash Indexed Table <- 
********************************/

hash_t hash_create(const size_t bytes);
hash_t hash_reserve(const size_t bytes, const size_t reserve);
index_t hash_search(const hash_t* table, const void* data);
index_t* hash_search_all(const hash_t* table, const void* data);
void hash_overload(hash_t* hash, size_t (*func)(const void*));
void* hash_index(const hash_t* table, const size_t index);
size_t hash_size(const hash_t* table);
size_t hash_capacity(const hash_t* table);
size_t hash_bytes(const hash_t* table);
void hash_resize(hash_t* table, const size_t new_size);
void hash_push(hash_t* table, const void* data);
size_t hash_push_if(hash_t* table, const void* data);
void hash_remove(hash_t* table, const void* data);
void hash_free(hash_t* table);

/***********************************
 -> Generic <Key, Value> Hash Map <- 
***********************************/

map_t map_create(const size_t key_size, const size_t value_size);
map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve);
index_t map_search(const map_t* map, const void* key);
index_t* map_search_all(const map_t* map, const void* key);
void map_overload(map_t* map, size_t (*hash_func)(const void*));
void* map_key_at(const map_t* map, const size_t index);
void* map_value_at(const map_t* map, const size_t index);
size_t map_size(const map_t* map);
size_t map_capacity(const map_t* map);
size_t map_key_bytes(const map_t* map);
size_t map_value_bytes(const map_t* map);
void map_resize(map_t* map, const size_t size);
void map_push(map_t* map, const void* key, const void* value);
size_t map_push_if(map_t* map, const void* key, const void* value);
void map_remove(map_t* map, const void* key);
void map_free(map_t* map);

/********************************
 -> Generic Doubly Linked List <- 
********************************/

list_t list_create(const size_t bytes);
size_t list_size(const list_t* list);
size_t list_bytes(const list_t* list);
void* list_index(const list_t* list, const size_t index);
lnode_t* list_search_node(const list_t* list, const void* data);
lnode_t* list_index_node(const list_t* list, const size_t index);
size_t list_search_index(const list_t* list, const void* data);
void list_push(list_t* list, const void* data);
void* list_pop(list_t* list);
void* list_pop_node(list_t* list, lnode_t* node);
void* list_pop_index(list_t* list, const size_t index);
void list_remove_node(list_t* list, lnode_t* node);
void list_remove_index(list_t* list, const size_t index);
void list_free(list_t* list);

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

lnode_t* lnode_create(const void* data, const size_t bytes);
void lnode_push(lnode_t* head, const void* data, const size_t bytes);
void* lnode_pop(lnode_t* node);
void lnode_remove(lnode_t* node);
size_t lnode_count(lnode_t* head);
lnode_t* lnode_search(lnode_t* head, const void* data, const size_t bytes);
size_t lnode_search_index(lnode_t* first, const void* data, const size_t bytes);
lnode_t* lnode_index_forward(lnode_t* head, const size_t index);
lnode_t* lnode_index_backward(lnode_t* tail, const size_t index, const size_t size);

/*************************
 -> Generic Binary Tree <- 
**************************/

btree_t btree_create(const size_t bytes);
void btree_free(btree_t* tree);

bnode_t* bnode_create(const void* data, const size_t bytes);
void bnode_connect(bnode_t* parent, const bnode_t* child);
void bnode_free(bnode_t* root);

/****************************
 -> Index Bucket Interface <- 
****************************/

size_t bucket_size(const bucket_t bucket);
size_t bucket_capacity(const bucket_t bucket);
index_t* bucket_data(const bucket_t bucket);

/*******************************
 -> Hash Functions Prototypes <- 
*******************************/

size_t hash_cstr(const void* key);
size_t hash_uint(const void* key);

#ifdef __cplusplus
}
#endif
#endif
