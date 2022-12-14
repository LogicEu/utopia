#include <utopia/list.h>
#include USTDLIB_H
#include USTRING_H

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

struct listnode* listnode_create(const void* data, const size_t bytes)
{
    struct listnode* node = malloc(sizeof(struct listnode));
    node->next = NULL;
    node->prev = NULL;
    node->data = malloc(bytes);
    memcpy(node->data, data, bytes);
    return node;
}

void listnode_push(struct listnode* head, const void* data, const size_t bytes)
{
    struct listnode* node = head;

    if (!node) {
        return;
    }
    
    while (node->next != NULL) {
        node = node->next;
    }
    
    node->next = listnode_create(data, bytes);
    node->next->prev = node;
}

void* listnode_pop(struct listnode* node)
{
    void* ret;
    struct listnode* next = node->next;
    struct listnode* prev = node->prev;
    
    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    ret = node->data;
    free(node);
    
    return ret;
}

void listnode_remove(struct listnode* node)
{
    struct listnode* next = node->next;
    struct listnode* prev = node->prev;
    
    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    free(node->data);
    free(node);
}

size_t listnode_count(struct listnode* first)
{
    size_t count = 0;
    struct listnode* node = first;
    while (node != NULL) {
        ++count;
        node = node->next;
    }
    return count;
}

struct listnode* listnode_search(struct listnode* head, const void* data, const size_t bytes)
{
    struct listnode* node = head;
    while (node != NULL) {
        if (memcmp(node->data, data, bytes)) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

size_t listnode_search_index(struct listnode* head, const void* data, const size_t bytes)
{
    size_t count = 0;
    struct listnode* node = head;
    while (node != NULL) {
        ++count;
        if (memcmp(node->data, data, bytes)) {
            return count;
        }
        node = node->next;
    }
    return 0;
}

struct listnode* listnode_index_forward(struct listnode* head, const size_t index)
{
    size_t count = 0;
    struct listnode* node = head;
    while (node != NULL) {
        if (count++ == index) {
            return node;
        }
        node = node->next;
    }
    return node;
}

struct listnode* listnode_index_backward(struct listnode* tail, const size_t index, const size_t size)
{
    size_t count = size - 1;
    struct listnode* node = tail;
    while (node != NULL) {
        if (count-- == index) {
            return node;
        }
        node = node->prev;
    }
    return node;
}

/********************************
 -> Generic Doubly Linked List <- 
********************************/

list_t list_create(const size_t bytes)
{
    list_t list;
    list.head = NULL;
    list.tail = NULL;
    list.bytes = bytes + !bytes;
    list.size = 0;
    return list;
}

size_t list_size(const list_t* list)
{
    return list->size;
}

size_t list_bytes(const list_t* list)
{
    return list->bytes;
}

struct listnode* list_index_node(const list_t* list, const size_t index)
{
    if (index <= list->size / 2) {
        return listnode_index_forward(list->head, index);
    }
    else return listnode_index_backward(list->tail, index, list->size);
}

void* list_index(const list_t* list, const size_t index)
{
    struct listnode* node = list_index_node(list, index);
    if (!node) {
        return NULL;
    }
    else return node->data;
}

void list_push(list_t* list, const void*data)
{   
    if (list->tail) {
        list->tail->next = listnode_create(data, list->bytes);
        list->tail = list->tail->next;
    }   

    else {
        list->head = listnode_create(data, list->bytes);
        list->tail = list->head;
    }
    
    ++list->size;
}

void* list_pop(list_t* list)
{
    return list_pop_node(list, list->tail);
}

struct listnode* list_search_node(const list_t* list, const void* data)
{
    return listnode_search(list->head, data, list->bytes);
}

size_t list_search_index(const list_t* list, const void* data)
{
    return listnode_search_index(list->head, data, list->bytes);
}

void* list_pop_index(list_t* list, const size_t index)
{
    return list_pop_node(list, list_index_node(list, index));
}

void* list_pop_node(list_t* list, struct listnode* node)
{
    void* ret;
    if (node == list->head) {
        list->head = node->next;
    }
    
    if (node == list->tail) {
        list->tail = node->prev;
    }
    
    ret = listnode_pop(node);
    list->size -= !!ret;
    
    return ret;
}

void list_remove_node(list_t* list, struct listnode* node)
{
    if (node == list->head) {
        list->head = node->next;
    }
    
    if (node == list->tail) {
        list->tail = node->prev;
    }
    
    listnode_remove(node);
    --list->size;
}

void list_remove_index(list_t* list, const size_t index)
{
    struct listnode* node = list_index_node(list, index);
    if (node) {
        list_remove_node(list, node);
    }
}

void list_free(list_t* list)
{
    struct listnode* node = list->head;
    while (node) {
        struct listnode* next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}
