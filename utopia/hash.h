#ifndef UTOPIA_HASH_H
#define UTOPIA_HASH_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

typedef struct hash {
    bucket_t* indices;
    void* data;
    size_t bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
} hash_t;

#define _hash_index(table, i) ((char*)(table)->data + (i) * (table)->bytes)

hash_t hash_create(const size_t bytes);
hash_t hash_reserve(const size_t bytes, const size_t reserve);
hash_t hash_copy(const hash_t* table);
index_t hash_search(const hash_t* table, const void* data);
void hash_overload(hash_t* hash, size_t (*func)(const void*));
void* hash_index(const hash_t* table, const size_t index);
size_t hash_size(const hash_t* table);
size_t hash_capacity(const hash_t* table);
size_t hash_bytes(const hash_t* table);
void hash_resize(hash_t* table, const size_t new_size);
void hash_push(hash_t* table, const void* data);
size_t hash_push_if(hash_t* table, const void* data);
void hash_remove(hash_t* table, const void* data);
void hash_free(hash_t* table);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_HASH_H */
