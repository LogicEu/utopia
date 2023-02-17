
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

#ifndef UTOPIA_TREE_H
#define UTOPIA_TREE_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/*********************
Generic & Dynamic Tree
*********************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct treenode {
    struct treenode** children;
    struct treenode* parent;
    void* data;
};

struct tree {  
    void* data;
    struct tree* parent;
    struct tree** children;
    struct treestore* store;
};

size_t treenode_children_count(const struct treenode* node);
size_t treenode_children_capacity(const size_t children_count);
struct treenode* treenode_create(const void* data, const size_t bytes);
struct treenode* treenode_root(struct treenode* node);
void treenode_children_free(struct treenode* node);
void treenode_free(struct treenode* node);
void treenode_push(struct treenode* root, struct treenode* leave);
int treenode_remove(struct treenode* root, const size_t index);

size_t tree_bytes(const struct tree* tree);
size_t tree_size(const struct tree* tree);
size_t tree_capacity(const struct tree* tree);
struct tree* tree_pool(const struct tree* tree);
void* tree_data_pool(const struct tree* tree);
size_t tree_children_count(const struct tree* node);
size_t tree_children_capacity(const size_t children_count);
struct tree* tree_create(const size_t bytes, const void* data);
struct tree* tree_spread(const struct tree* tree, const void* data);
struct tree* tree_reserve(const size_t bytes, const size_t reserve, const void* data);
struct tree* tree_push(struct tree* node, const void* data);
struct tree* tree_root(struct tree* tree);
void tree_free(struct tree* tree);
void tree_clean(struct tree* tree);
void tree_insert(struct tree* parent, struct tree* child);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_TREE_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_TREE_IMPLEMENTED
#define UTOPIA_TREE_IMPLEMENTED

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

/*********************
Generic & Dynamic Tree
*********************/

size_t treenode_children_count(const struct treenode* node)
{
    size_t i;
    for (i = 0; node->children[i]; ++i);
    return i;
}

size_t treenode_children_capacity(const size_t children_count)
{
    size_t i;
    for (i = 1; i < children_count; i <<= 1);
    return i;
}

struct treenode* treenode_create(const void* data, const size_t bytes)
{
    struct treenode* node = malloc(sizeof(struct treenode));
    node->parent = NULL;
    node->children = calloc(sizeof(struct treenode*), 1);
    node->data = malloc(bytes);
    memcpy(node->data, data, bytes);
    return node;
}

struct treenode* treenode_root(struct treenode* node)
{
    while (node->parent) {
        node = node->parent;
    }
    return node;
}

void treenode_children_free(struct treenode* node)
{
    size_t i;
    for (i = 0; node->children[i]; ++i) {
        treenode_free(node->children[i]);
    }
    
    node->children = realloc(node->children, sizeof(struct treenode*));
    *node->children = NULL;
}

void treenode_free(struct treenode* node)
{
    size_t i;
    for (i = 0; node->children[i]; ++i) {
        treenode_free(node->children[i]);
    }
    
    free(node->children);
    free(node->data);
    
    node->parent = NULL;
    node->children = NULL;
    node->data = NULL;
    
    free(node);
}

void treenode_push(struct treenode* root, struct treenode* leaf)
{
    const size_t size = treenode_children_count(root);
    const size_t cap = treenode_children_capacity(size + 1);
    
    if (size + 2 >= cap) {
        root->children = realloc(root->children, cap * 2 * sizeof(struct treenode*));
    }
    
    leaf->parent = root;
    root->children[size] = leaf;
    root->children[size + 1] = NULL;
}

int treenode_remove(struct treenode* root, const size_t index)
{
    const size_t count = treenode_children_count(root);
    if (index < count) {
        treenode_free(root->children[index]);
        memmove(
            root->children + index, 
            root->children + index + 1, 
            (count - index) * sizeof(struct treenode*)
        );
        return 1;
    }
    return 0;
}

/* Tree arena opaque struct */

struct treestore {
    struct tree* nodes;
    void* data;
    size_t bytes;
    size_t size;
    size_t capacity;
};


static struct treestore* treestore_create(const size_t bytes)
{
    struct treestore* store = malloc(sizeof(struct treestore));
    store->nodes = NULL;
    store->data = NULL;
    store->bytes = bytes;
    store->capacity = 0;
    store->size = 0;
    return store;
}

static struct treestore* treestore_reserve(const size_t bytes, const size_t reserve)
{
    struct treestore* store = malloc(sizeof(struct treestore));
    store->nodes = malloc(reserve * sizeof(struct tree));
    store->data = malloc(reserve * bytes);
    store->bytes = bytes;
    store->capacity = reserve;
    store->size = 0;
    return store;
}

static void treestore_free(struct treestore* store)
{
    if (store->capacity) {
        free(store->nodes);
        free(store->data);
    }
    free(store);
}

static void treestore_offset(
    struct treestore* store, const ptrdiff_t offnode, const ptrdiff_t offdata)
{
    size_t i, j;
    struct tree* node;
    const size_t count = store->size;

    node = store->nodes;
    for (i = 0; i < count; ++i) {  
        node->data = (void*)((ptrdiff_t)node->data + offdata);
        if (node->parent)
            node->parent = (struct tree*)((ptrdiff_t)node->parent + offnode);
        for (j = 0; node->children[j]; ++j)
            node->children[j] = (struct tree*)((ptrdiff_t)node->children[j] + offnode);
        ++node;
    }
}

static void treestore_clean_back(struct treestore* store)
{
    const struct tree nulltree = {NULL, NULL, NULL, NULL};
    const struct tree *end, *node = store->nodes + store->size - 1;
    for (end = store->nodes; node >= end; --node) {
        if (memcmp(node, &nulltree, sizeof(struct tree)))
            break;
        --store->size;
    }
}

static void treestore_clean_front(struct treestore* store)
{
    ptrdiff_t offset;
    const struct tree nulltree = {NULL, NULL, NULL, NULL};
    const struct tree *end, *node = store->nodes;
    for (end = store->nodes + store->size; node < end; ++node) {
        if (memcmp(node, &nulltree, sizeof(struct tree)))
            break;
    }

    offset = node - node->store->nodes;
    if (offset) {
        const size_t bytes = store->bytes, size = store->size - offset;
        memmove(store->data, (char*)store->data + bytes * offset, size * bytes);
        memmove(store->nodes, store->nodes + offset, size * sizeof(struct tree));
        treestore_offset(store, offset * sizeof(struct tree), offset * store->bytes);
        store->size = size;
    }
}

static struct tree* tree_store(struct treestore* store, const void* data)
{
    struct tree* node;
    if (store->size + 1 >= store->capacity) {
        void* newdata;
        ptrdiff_t offnode, offdata;
        
        store->capacity = (!store->capacity + store->capacity) * 4;
        node = realloc(store->nodes, store->capacity * sizeof(struct tree));
        newdata = realloc(store->data, store->capacity * store->bytes);
        
        offnode = (ptrdiff_t)node - (ptrdiff_t)store->nodes;
        offdata = (ptrdiff_t)newdata - (ptrdiff_t)store->data;
        
        store->nodes = node;
        store->data = newdata;
        
        if (store->size) 
            treestore_offset(store, offnode, offdata);
    }

    node = store->nodes + store->size;
    node->store = store;
    node->parent = NULL;
    node->data = (char*)store->data + store->size * store->bytes;
    node->children = calloc(sizeof(struct tree*), 1);
    ++store->size;

    if (data)
        memcpy(node->data, data, store->bytes);
    else memset(node->data, 0, store->bytes);
    
    return node;
}

/* Self Managing Tree API */

size_t tree_bytes(const struct tree* tree)
{
    return tree->store->bytes;
}

size_t tree_size(const struct tree* tree)
{
    return tree->store->size;
}

size_t tree_capacity(const struct tree* tree)
{
    return tree->store->capacity;
}

void* tree_data(const struct tree* tree)
{
    return tree->data;
}

void* tree_data_pool(const struct tree* tree)
{
    return tree->store->data;
}

struct tree* tree_pool(const struct tree* tree)
{
    return tree->store->nodes;
}

size_t tree_children_count(const struct tree* node)
{
    size_t i;
    for (i = 0; node->children[i]; ++i);
    return i;
}

size_t tree_children_capacity(const size_t children_count)
{
    size_t i;
    for (i = 1; i < children_count; i <<= 1);
    return i;
}

struct tree* tree_create(const size_t bytes, const void* data)
{
    struct treestore* store = treestore_create(bytes);
    return tree_store(store, data);
}

struct tree* tree_reserve(const size_t bytes, const size_t reserve, const void* data)
{
    struct treestore* store = treestore_reserve(bytes, reserve);
    return tree_store(store, data);
}

struct tree* tree_spread(const struct tree* tree, const void* data)
{
    return tree_store(tree->store, data);
}

void tree_free(struct tree* tree)
{
    struct treestore* store = tree->store;
    if (!tree->parent) {
        free(tree->children);
        memset(tree, 0, sizeof(struct tree));
        treestore_free(store);
    } else {
        size_t i;
        for (i = 0; tree->children[i]; ++i)
            tree_free(tree->children[i]);
        free(tree->children);
        memset(tree, 0, sizeof(struct tree));
        treestore_clean_back(store);
    }
}

void tree_clean(struct tree* tree)
{
    treestore_clean_back(tree->store);
    treestore_clean_front(tree->store);
}

void tree_insert(struct tree* parent, struct tree* child)
{
    const size_t size = tree_children_count(parent);
    const size_t cap = tree_children_capacity(size + 1);

    if (size + 2 >= cap) {
        parent->children = realloc(parent->children, cap * 2 * sizeof(struct tree*));
    }

    child->parent = parent;
    parent->children[size] = child;
    parent->children[size + 1] = NULL; 
}

struct tree* tree_push(struct tree* node, const void* data)
{
    struct tree* child;
    const ptrdiff_t offset = node - node->store->nodes;
    child = tree_store(node->store, data);
    node = node->store->nodes + offset;
    tree_insert(node, child);
    return node;
}

struct tree* tree_root(struct tree* tree)
{
    while (tree->parent) {
        tree = tree->parent;
    }
    return tree;
}

#endif /* UTOPIA_TREE_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
