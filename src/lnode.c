#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

lnode_t* lnode_create(const void* data, const size_t bytes)
{
    lnode_t* node = malloc(sizeof(lnode_t));
    node->next = NULL;
    node->prev = NULL;
    node->data = malloc(bytes);
    memcpy(node->data, data, bytes);
    return node;
}

void lnode_push(lnode_t* restrict head, const void* restrict data, const size_t bytes)
{
    if (!head) {
        return;
    }
    
    lnode_t* node = head;
    while (node->next != NULL) {
        node = node->next;
    }
    
    node->next = lnode_create(data, bytes);
    node->next->prev = node;
}

void* lnode_pop(lnode_t* restrict node)
{
    lnode_t* next = node->next;
    lnode_t* prev = node->prev;
    
    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    void* ret = node->data;
    free(node);
    
    return ret;
}

void lnode_remove(lnode_t* restrict node)
{
    lnode_t* next = node->next;
    lnode_t* prev = node->prev;
    
    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    free(node->data);
    free(node);
}

size_t lnode_count(lnode_t* restrict first)
{
    size_t count = 0;
    lnode_t* node = first;
    while (node != NULL) {
        ++count;
        node = node->next;
    }
    return count;
}

lnode_t* lnode_search(lnode_t* restrict head, const void* restrict data, const size_t bytes)
{
    lnode_t* node = head;
    while (node != NULL) {
        if (memcmp(node->data, data, bytes)) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

size_t lnode_search_index(lnode_t* restrict head, const void* restrict data, const size_t bytes)
{
    size_t count = 0;
    lnode_t* node = head;
    while (node != NULL) {
        ++count;
        if (memcmp(node->data, data, bytes)) {
            return count;
        }
        node = node->next;
    }
    return 0;
}

lnode_t* lnode_index_forward(lnode_t* restrict head, const size_t index)
{
    size_t count = 0;
    lnode_t* node = head;
    while (node != NULL) {
        if (count++ == index) {
            return node;
        }
        node = node->next;
    }
    return node;
}

lnode_t* lnode_index_backward(lnode_t* restrict tail, const size_t index, const size_t size)
{
    size_t count = size - 1;
    lnode_t* node = tail;
    while (node != NULL) {
        if (count-- == index) {
            return node;
        }
        node = node->prev;
    }
    return node;
}