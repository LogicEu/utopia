#ifndef UTOPIA_LIST_H
#define UTOPIA_LIST_H

#include <utopia/types.h>

struct listnode {
    struct listnode *next;
    struct listnode *prev;
    void* data;
};

typedef struct list {
    struct listnode *head;
    struct listnode *tail;
    size_t bytes;
    size_t size;
} list_t;

struct listnode* lnode_create(const void* data, const size_t bytes);
void lnode_push(struct listnode* head, const void* data, const size_t bytes);
void* lnode_pop(struct listnode* node);
void lnode_remove(struct listnode* node);
size_t lnode_count(struct listnode* head);
struct listnode* lnode_search(struct listnode* head, const void* data, const size_t bytes);
size_t lnode_search_index(struct listnode* first, const void* data, const size_t bytes);
struct listnode* lnode_index_forward(struct listnode* head, const size_t index);
struct listnode* lnode_index_backward(struct listnode* tail, const size_t index, const size_t size);

list_t list_create(const size_t bytes);
size_t list_size(const list_t* list);
size_t list_bytes(const list_t* list);
void* list_index(const list_t* list, const size_t index);
struct listnode* list_search_node(const list_t* list, const void* data);
struct listnode* list_index_node(const list_t* list, const size_t index);
size_t list_search_index(const list_t* list, const void* data);
void list_push(list_t* list, const void* data);
void* list_pop(list_t* list);
void* list_pop_node(list_t* list, struct listnode* node);
void* list_pop_index(list_t* list, const size_t index);
void list_remove_node(list_t* list, struct listnode* node);
void list_remove_index(list_t* list, const size_t index);
void list_free(list_t* list);

#endif /* UTOPIA_LIST_H */
