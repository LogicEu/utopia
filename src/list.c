#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/********************************
 -> Generic Doubly Linked List <- 
********************************/

list_t list_create(const size_t bytes)
{
    list_t list;
    list.head = NULL;
    list.tail = NULL;
    list.bytes = bytes;
    list.size = 0;
    return list;
}

list_t* list_new(const size_t bytes)
{
    list_t* list = malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;
    list->bytes = bytes;
    list->size = 0;
    return list;
}

size_t list_is_empty(const list_t* restrict list)
{
    return list->head == NULL;
}

void* list_index(const list_t* restrict list, const size_t index)
{
    return list_index_node(list, index)->data;
}

node_t* list_index_node(const list_t* restrict list, const size_t index)
{
    if (index <= list->size / 2) return node_index_forward(list->head, index);
    return node_index_backward(list->tail, index, list->size);
}

void list_push(list_t* list, const void* data)
{
    void* new_data = malloc(list->bytes);
    memcpy(new_data, data, list->bytes);
    if (list->tail != NULL) {
        list->tail->next = node_new(new_data);
        list->tail = list->tail->next;
    } else {
        list->head = node_new(new_data);
        list->tail = list->head;
    }
    ++list->size;
}

void* list_pop(list_t* list)
{
    return list_pop_node(list, list->tail);
}

node_t* list_find_node(const list_t* restrict list, const void* data)
{
    return node_find(list->head, data, list->bytes);
}

size_t list_find_index(const list_t* restrict list, const void* data)
{
    return node_find_index(list->head, data, list->bytes);
}

void* list_pop_index(list_t* list, const size_t index)
{
    return list_pop_node(list, list_index_node(list, index));
}

void* list_pop_node(list_t* list, node_t* node)
{
    if (node == list->head) list->head = node->next;
    if (node == list->tail) list->tail = node->prev;
    void* ret = node_pop(node);
    --list->size;
    return ret;
}

void list_remove_node(list_t* list, node_t* node)
{
    if (node == list->head) list->head = node->next;
    if (node == list->tail) list->tail = node->prev;
    node_remove(node);
    list->size--;
}

void list_remove_index(list_t* list, const size_t index)
{
    node_t* node = list_index_node(list, index);
    list_remove_node(list, node);
}

void list_free(list_t* list)
{
    node_t* node = list->head;
    while (node != NULL) {
        node_t* next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_destroy(list_t* list)
{
    if (list == NULL) return;
    list_free(list);
    free(list);
}
