#ifndef UTOPIA_TREE_H
#define UTOPIA_TREE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct treenode {
    struct treenode** children;
    struct treenode* parent;
    void* data;
};

struct tree {
    struct treenode* root;
    size_t bytes;
};

size_t treenode_children_count(const struct treenode* node);
size_t treenode_children_capacity(const size_t children_count);
struct treenode* treenode_create(const void* data, const size_t bytes);
struct treenode* treenode_root(struct treenode* node);
void treenode_free(struct treenode* node);
void treenode_push(struct treenode* root, struct treenode* leave);
int treenode_remove(struct treenode* root, const size_t index);

struct tree tree_create(const size_t bytes);
void tree_free(struct tree* tree);
void tree_push_root(struct tree* tree, const void* data);
void tree_push_leave(struct tree* tree, const void* data);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_TREE_H */
