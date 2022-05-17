#include <utopia.h>
#include <stdlib.h>
#include <string.h>

btree_t btree_create(const size_t bytes)
{
    btree_t tree;
    tree.bytes = bytes + !bytes;
    tree.root = NULL;
    return tree;
}

void btree_free(btree_t* tree)
{
    if (tree->root) {
        bnode_free(tree->root);
    }
}