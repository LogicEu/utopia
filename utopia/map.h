#ifndef UTOPIA_MAP_H
#define UTOPIA_MAP_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

typedef struct map {
    bucket_t* indices;
    void* keys;
    void* values;
    size_t key_bytes;
    size_t value_bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
} map_t;

#define _map_key_at(map, i) ((char*)(map)->keys + (map)->key_bytes * (i))
#define _map_value_at(map, i) ((char*)(map)->values + (map)->value_bytes * (i))

map_t map_create(const size_t key_size, const size_t value_size);
map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve);
map_t map_copy(const map_t* map);
index_t map_search(const map_t* map, const void* key);
void map_overload(map_t* map, size_t (*hash_func)(const void*));
void* map_key_at(const map_t* map, const size_t index);
void* map_value_at(const map_t* map, const size_t index);
size_t map_size(const map_t* map);
size_t map_capacity(const map_t* map);
size_t map_key_bytes(const map_t* map);
size_t map_value_bytes(const map_t* map);
void map_resize(map_t* map, const size_t size);
void map_push(map_t* map, const void* key, const void* value);
size_t map_push_if(map_t* map, const void* key, const void* value);
void map_remove(map_t* map, const void* key);
void map_free(map_t* map);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_MAP_H */
