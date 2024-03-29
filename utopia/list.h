
/*  Copyright (c) 2022 Eugenio Arteaga A.

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef UTOPIA_LIST_H
#define UTOPIA_LIST_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/*************************
Generic Doubly Linked List
*************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

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

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_LIST_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_LIST_IMPLEMENTED
#define UTOPIA_LIST_IMPLEMENTED

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

/*************************
Generic Doubly Linked List
*************************/

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

struct list list_create(const size_t bytes)
{
    struct list list;
    list.head = NULL;
    list.tail = NULL;
    list.bytes = bytes + !bytes;
    list.size = 0;
    return list;
}

size_t list_size(const struct list* list)
{
    return list->size;
}

size_t list_bytes(const struct list* list)
{
    return list->bytes;
}

struct listnode* list_index_node(const struct list* list, const size_t index)
{
    if (index <= list->size / 2) {
        return listnode_index_forward(list->head, index);
    }
    else return listnode_index_backward(list->tail, index, list->size);
}

void* list_index(const struct list* list, const size_t index)
{
    struct listnode* node = list_index_node(list, index);
    if (!node) {
        return NULL;
    }
    else return node->data;
}

void list_push(struct list* list, const void*data)
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

void* list_pop(struct list* list)
{
    return list_pop_node(list, list->tail);
}

struct listnode* list_search_node(const struct list* list, const void* data)
{
    return listnode_search(list->head, data, list->bytes);
}

size_t list_search_index(const struct list* list, const void* data)
{
    return listnode_search_index(list->head, data, list->bytes);
}

void* list_pop_index(struct list* list, const size_t index)
{
    return list_pop_node(list, list_index_node(list, index));
}

void* list_pop_node(struct list* list, struct listnode* node)
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

void list_remove_node(struct list* list, struct listnode* node)
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

void list_remove_index(struct list* list, const size_t index)
{
    struct listnode* node = list_index_node(list, index);
    if (node) {
        list_remove_node(list, node);
    }
}

void list_free(struct list* list)
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

#endif /* UTOPIA_LIST_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
