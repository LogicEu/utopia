#ifndef UTOPIA_MAP_H
#define UTOPIA_MAP_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct map {
    bucket_t* indices;
    void* keys;
    void* values;
    size_t key_bytes;
    size_t value_bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
};

#define _map_key_at(map, i) ((char*)(map)->keys + (map)->key_bytes * (i))
#define _map_value_at(map, i) ((char*)(map)->values + (map)->value_bytes * (i))

struct map map_create(const size_t key_size, const size_t value_size);
struct map map_reserve(const size_t key_size, const size_t value_size, const size_t reserve);
struct map map_copy(const struct map* map);
index_t map_search(const struct map* map, const void* key);
void map_overload(struct map* map, size_t (*hash_func)(const void*));
void* map_key_at(const struct map* map, const size_t index);
void* map_value_at(const struct map* map, const size_t index);
size_t map_size(const struct map* map);
size_t map_capacity(const struct map* map);
size_t map_key_bytes(const struct map* map);
size_t map_value_bytes(const struct map* map);
void map_resize(struct map* map, const size_t size);
void map_push(struct map* map, const void* key, const void* value);
size_t map_push_if(struct map* map, const void* key, const void* value);
void map_remove(struct map* map, const void* key);
void map_free(struct map* map);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_MAP_H */
