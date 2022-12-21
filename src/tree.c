#include <utopia/tree.h>
#include USTDLIB_H
#include USTRING_H

/******************************
 -> Generic Tree Node Struct <- 
******************************/

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

void treenode_free(struct treenode* node)
{
    size_t i;
    for (i = 0; node->children[i]; ++i) {
        treenode_free(node->children[i]);
    }

    free(node->children);
    free(node->data);
    
    memset(node, 0, sizeof(struct treenode));
    free(node);
}

void treenode_push(struct treenode* root, struct treenode* leave)
{
    const size_t size = treenode_children_count(root);
    const size_t cap = treenode_children_capacity(size);

    if (size + 1 == cap) {
        root->children = realloc(root->children, cap * 2 * sizeof(struct treenode));
    }
    
    leave->parent = root;
    root->children[size] = leave;
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

/****************************
 -> Generic Tree Container <- 
****************************/

tree_t tree_create(const size_t bytes)
{
    tree_t tree;
    tree.root = NULL;
    tree.bytes = bytes;
    return tree;
}

void tree_free(tree_t* tree)
{
    treenode_free(tree->root);
    tree->root = NULL;
}

void tree_push_root(tree_t* tree, const void* data)
{
    struct treenode* node = treenode_create(data, tree->bytes);
    if (tree->root) {
        treenode_push(node, tree->root);
    }
    tree->root = node;
}

void tree_push_leave(tree_t* tree, const void* data)
{
    struct treenode* node = treenode_create(data, tree->bytes);
    if (tree->root) {
        treenode_push(tree->root, node);
    } else {
        tree->root = node;
    }
}
