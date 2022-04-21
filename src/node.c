#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

node_t* node_create(void* restrict data)
{
    node_t* node = malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void node_push(node_t* restrict head, void* restrict data)
{
    if (!head) {
        return;
    }
    
    node_t* node = head;
    while (node->next != NULL) {
        node = node->next;
    }
    
    node->next = node_create(data);
    node->next->prev = node;
}

void* node_pop(node_t* restrict node)
{
    node_t* next = node->next;
    node_t* prev = node->prev;
    
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

void node_remove(node_t* restrict node)
{
    node_t* next = node->next;
    node_t* prev = node->prev;
    
    if (prev) {
        prev->next = next;
    }

    if (next) {
        next->prev = prev;
    }

    free(node->data);
    free(node);
}

size_t node_count(node_t* restrict first)
{
    size_t count = 0;
    node_t* node = first;
    while (node != NULL) {
        ++count;
        node = node->next;
    }
    return count;
}

node_t* node_find(node_t* restrict head, const void* restrict data, const size_t bytes)
{
    node_t* node = head;
    while (node != NULL) {
        if (memcmp(node->data, data, bytes)) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

size_t node_find_index(node_t* restrict head, const void* restrict data, const size_t bytes)
{
    size_t count = 0;
    node_t* node = head;
    while (node != NULL) {
        ++count;
        if (memcmp(node->data, data, bytes)) {
            return count;
        }
        node = node->next;
    }
    return 0;
}

node_t* node_index_forward(node_t* restrict head, const size_t index)
{
    size_t count = 0;
    node_t* node = head;
    while (node != NULL) {
        if (count++ == index) {
            return node;
        }
        node = node->next;
    }
    return node;
}

node_t* node_index_backward(node_t* restrict tail, const size_t index, const size_t size)
{
    size_t count = size - 1;
    node_t* node = tail;
    while (node != NULL) {
        if (count-- == index) {
            return node;
        }
        node = node->prev;
    }
    return node;
}