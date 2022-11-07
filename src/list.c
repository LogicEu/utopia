#include <utopia.h>
#include USTDLIB_H
#include USTRING_H

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

size_t list_size(const list_t* restrict list)
{
    return list->size;
}

size_t list_bytes(const list_t* restrict list)
{
    return list->bytes;
}

lnode_t* list_index_node(const list_t* restrict list, const size_t index)
{
    if (index <= list->size / 2) {
        return lnode_index_forward(list->head, index);
    }
    else return lnode_index_backward(list->tail, index, list->size);
}

void* list_index(const list_t* restrict list, const size_t index)
{
    lnode_t* node = list_index_node(list, index);
    if (!node) {
        return NULL;
    }
    else return node->data;
}

void list_push(list_t* restrict list, const void* restrict data)
{   
    if (list->tail) {
        list->tail->next = lnode_create(data, list->bytes);
        list->tail = list->tail->next;
    }   

    else {
        list->head = lnode_create(data, list->bytes);
        list->tail = list->head;
    }
    
    ++list->size;
}

void* list_pop(list_t* list)
{
    return list_pop_node(list, list->tail);
}

lnode_t* list_search_node(const list_t* restrict list, const void* data)
{
    return lnode_search(list->head, data, list->bytes);
}

size_t list_search_index(const list_t* restrict list, const void* data)
{
    return lnode_search_index(list->head, data, list->bytes);
}

void* list_pop_index(list_t* restrict list, const size_t index)
{
    return list_pop_node(list, list_index_node(list, index));
}

void* list_pop_node(list_t* restrict list, lnode_t* restrict node)
{
    if (node == list->head) {
        list->head = node->next;
    }
    
    if (node == list->tail) {
        list->tail = node->prev;
    }
    
    void* ret = lnode_pop(node);
    list->size -= !!ret;
    
    return ret;
}

void list_remove_node(list_t* restrict list, lnode_t* restrict node)
{
    if (node == list->head) {
        list->head = node->next;
    }
    
    if (node == list->tail) {
        list->tail = node->prev;
    }
    
    lnode_remove(node);
    --list->size;
}

void list_remove_index(list_t* list, const size_t index)
{
    lnode_t* node = list_index_node(list, index);
    if (node) {
        list_remove_node(list, node);
    }
}

void list_free(list_t* list)
{
    lnode_t* node = list->head;
    while (node) {
        lnode_t* next = node->next;
        free(node->data);
        free(node);
        node = next;
    }
    
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}
