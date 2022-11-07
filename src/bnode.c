#include <utopia.h>
#include USTDLIB_H
#include USTRING_H

bnode_t* bnode_create(const void* data, const size_t bytes)
{
    bnode_t* node = malloc(sizeof(bnode_t));
    node->left = NULL;
    node->right = NULL;
    node->data = malloc(bytes);
    memcpy(node->data, data, bytes);
    return node;
}

void bnode_connect(bnode_t* restrict parent, const bnode_t* restrict child)
{
    if (!parent->left) {
        parent->left = (bnode_t*)(size_t)child;
    }
    else if (!parent->right) {
        parent->right = (bnode_t*)(size_t)child;
    }
}

void bnode_free(bnode_t* root)
{
    if (root) {
        bnode_free(root->left);
        bnode_free(root->right);
        root->left = root->right = NULL;

        if (root->data) {
            free(root->data);
        }

        free(root);
    }
}