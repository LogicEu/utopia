#include <Utopia.h>
#include <stdlib.h>
#include <string.h>

/********************************
 -> Doubly Linked Generic Node <- 
********************************/

node_t* node_new(void* data)
{
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void node_push(node_t* head, void* data)
{
    if (head == NULL) return;
    node_t* node = head;
    while (node->next != NULL) {
        node = node->next;
    }
    node->next = node_new(data);
    node->next->prev = node;
}

void* node_pop(node_t* node)
{
    node_t* next = node->next;
    node_t* prev = node->prev;
    if (prev != NULL) prev->next = next;
    if (next != NULL) next->prev = prev;
    void* ret = node->data;
    free(node);
    return ret;
}

void node_remove(node_t* node)
{
    node_t* next = node->next;
    node_t* prev = node->prev;
    if (prev != NULL) prev->next = next;
    if (next != NULL) next->prev = prev;
    free(node->data);
    free(node);
}

unsigned int node_count(node_t* first)
{
    unsigned int count = 0;
    node_t* node = first;
    while (node != NULL) {
        count++;
        node = node->next;
    }
    return count;
}

node_t* node_find(node_t* head, void* data, unsigned int bytes)
{
    node_t* node = head;
    while (node != NULL) {
        if (memcmp(node->data, data, bytes)) return node;
        node = node->next;
    }
    return NULL;
}

int node_find_index(node_t* head, void* data, unsigned int bytes)
{
    int count = 0;
    node_t* node = head;
    while (node != NULL) {
        if (memcmp(node->data, data, bytes)) return count;
        count++;
        node = node->next;
    }
    return -1;
}

node_t* node_index_forward(node_t* head, unsigned int index)
{
    unsigned int count = 0;
    node_t* node = head;
    while (node != NULL) {
        if (count++ == index) return node;
        node = node->next;
    }
    return node;
}

node_t* node_index_backward(node_t* tail, unsigned int index, unsigned int size)
{
    unsigned int count = size - 1;
    node_t* node = tail;
    while (node != NULL) {
        if (count-- == index) return node;
        node = node->prev;
    }
    return node;
}