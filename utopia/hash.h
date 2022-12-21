#ifndef UTOPIA_HASH_H
#define UTOPIA_HASH_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct hash {
    bucket_t* indices;
    void* data;
    size_t bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
};

#define _hash_index(table, i) ((char*)(table)->data + (i) * (table)->bytes)

struct hash hash_create(const size_t bytes);
struct hash hash_reserve(const size_t bytes, const size_t reserve);
struct hash hash_copy(const struct hash* table);
index_t hash_search(const struct hash* table, const void* data);
void hash_overload(struct hash* hash, size_t (*func)(const void*));
void* hash_index(const struct hash* table, const size_t index);
size_t hash_size(const struct hash* table);
size_t hash_capacity(const struct hash* table);
size_t hash_bytes(const struct hash* table);
void hash_resize(struct hash* table, const size_t new_size);
void hash_push(struct hash* table, const void* data);
size_t hash_push_if(struct hash* table, const void* data);
void hash_remove(struct hash* table, const void* data);
void hash_free(struct hash* table);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_HASH_H */
