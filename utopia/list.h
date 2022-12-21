#ifndef UTOPIA_LIST_H
#define UTOPIA_LIST_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct listnode {
    struct listnode *next;
    struct listnode *prev;
    void* data;
};

struct list {
    struct listnode *head;
    struct listnode *tail;
    size_t bytes;
    size_t size;
};

struct listnode* listnode_create(const void* data, const size_t bytes);
void listnode_push(struct listnode* head, const void* data, const size_t bytes);
void* listnode_pop(struct listnode* node);
void listnode_remove(struct listnode* node);
size_t listnode_count(struct listnode* head);
struct listnode* listnode_search(struct listnode* head, const void* data, const size_t bytes);
size_t listnode_search_index(struct listnode* first, const void* data, const size_t bytes);
struct listnode* listnode_index_forward(struct listnode* head, const size_t index);
struct listnode* listnode_index_backward(struct listnode* tail, const size_t index, const size_t size);

struct list list_create(const size_t bytes);
size_t list_size(const struct list* list);
size_t list_bytes(const struct list* list);
void* list_index(const struct list* list, const size_t index);
struct listnode* list_search_node(const struct list* list, const void* data);
struct listnode* list_index_node(const struct list* list, const size_t index);
size_t list_search_index(const struct list* list, const void* data);
void list_push(struct list* list, const void* data);
void* list_pop(struct list* list);
void* list_pop_node(struct list* list, struct listnode* node);
void* list_pop_index(struct list* list, const size_t index);
void list_remove_node(struct list* list, struct listnode* node);
void list_remove_index(struct list* list, const size_t index);
void list_free(struct list* list);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_LIST_H */
