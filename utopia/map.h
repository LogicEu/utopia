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

/****************************
Generic <Key, Value> Hash Map
*****************************/

#ifdef UTOPIA_MAP_IMPL

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include <utopia/bucket.h>
#include USTDLIB_H
#include USTRING_H

static void* memdup(const void* src, size_t size)
{
    void* dup = malloc(size);
    memcpy(dup, src, size);
    return dup;
}

static size_t hash_cstr(const void* key)
{
    int c;
    size_t hash = 5381;
    const unsigned char* str = key;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

struct map map_create(const size_t key_size, const size_t value_size)
{
    struct map map;
    map.indices = NULL;
    map.keys = NULL;
    map.values = NULL;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.size = 0;
    map.mod = 0;
    map.func = &hash_cstr;
    return map;
}

struct map map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    struct map map;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.indices = reserve ? calloc(reserve, sizeof(bucket_t)) : NULL;
    map.keys = reserve ? malloc(reserve * map.key_bytes) : NULL;
    map.values = reserve ? malloc(reserve * map.value_bytes) : NULL;
    map.mod = reserve;
    map.size = 0;
    map.func = &hash_cstr;
    return map;
}

struct map map_copy(const struct map* map)
{
    struct map m = *map;
    if (map->mod) {
        size_t i, size;

        m.keys = memdup(map->keys, map->mod * map->key_bytes);
        m.values = memdup(map->values, map->mod * map->value_bytes);
        m.indices = memdup(map->indices, map->mod * sizeof(bucket_t));
        
        for (i = 0; i < map->mod; ++i) {
            size = bucket_size(map->indices[i]);
            if (size) {
                size += BUCKET_DATA_INDEX;
                m.indices[i] = memdup(map->indices[i], size * sizeof(index_t));
            }
        }
    }
    return m;
}

size_t map_capacity(const struct map* map)
{
    return map->mod;
}

void map_overload(struct map* map, size_t (*func)(const void*))
{
    map->func = func;
}

void* map_key_at(const struct map* map, const size_t index)
{
    return _map_key_at(map, index);
}

void* map_value_at(const struct map* map, const size_t index)
{
    return _map_value_at(map, index);
}

size_t map_size(const struct map* map)
{
    return map->size;
}

size_t map_key_bytes(const struct map* map)
{
    return map->key_bytes;
}

size_t map_value_bytes(const struct map* map)
{
    return map->value_bytes;
}

index_t map_search(const struct map* map, const void* data)
{
    if (map->mod) {
    
        size_t i;
        const size_t hash = map->func(data);
        bucket_t bucket = map->indices[hash % map->mod];
    
        const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _map_key_at(map, bucket[i]);
            if (hash == map->func(k)) {
                return bucket[i] + 1;
            }
        }
    }
    
    return 0;
}

void map_resize(struct map* map, const size_t new_size)
{
    size_t i;
    const char* key;
    const size_t size = map->size;
    const size_t bytes = map->key_bytes;

    if (map->mod) {
        buckets_free(map->indices, map->mod);
    }

    map->mod = new_size + !new_size * UTOPIA_HASH_SIZE;
    map->keys = realloc(map->keys, map->mod * map->key_bytes);
    map->values = realloc(map->values, map->mod * map->value_bytes);
    map->indices = realloc(map->indices, map->mod * sizeof(bucket_t));
    memset(map->indices, 0, map->mod * sizeof(bucket_t));
    
    key = map->keys;
    for (i = 0; i < size; ++i, key += bytes) {
        const size_t hash_mod = map->func(key) % map->mod;
        map->indices[hash_mod] = bucket_push(map->indices[hash_mod], i);
    }
}

void map_remove(struct map* map, const void* key)
{
    if (map->mod) {

        size_t i, search = 0;
        const size_t hash = map->func(key);
        bucket_t bucket = map->indices[hash % map->mod];      

        const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _map_key_at(map, bucket[i]);
            if (hash == map->func(k)) {
                search = i;
                break;
            }
        }
        
        if (search) {
            const index_t find = bucket[search];
            
            char* key = _map_key_at(map, find);
            char* val = _map_value_at(map, find);
            memmove(key, key + map->key_bytes, (map->size - find - 1) * map->key_bytes);
            memmove(val, val + map->value_bytes, (map->size - find - 1) * map->value_bytes);
            
            bucket_remove(bucket, search);
            buckets_reindex(map->indices, map->mod, find);
            --map->size;
        }
    }
}

void map_push(struct map* map, const void* key, const void* value)
{
    size_t hashmod;
    if (map->size == map->mod) {
        map_resize(map, map->mod * 2);
    }

    hashmod = map->func(key) % map->mod;
    map->indices[hashmod] = bucket_push(map->indices[hashmod], map->size);

    memcpy(_map_key_at(map, map->size), key, map->key_bytes);
    memcpy(_map_value_at(map, map->size), value, map->value_bytes);
    ++map->size;
}

size_t map_push_if(struct map* map, const void* key, const void* value)
{
    const size_t index = map_search(map, key);
    if (index) {
        return index;
    }

    map_push(map, key, value);
    return 0;
}

void map_free(struct map* map)
{
    if (map->indices) {
        buckets_free(map->indices, map->mod);
        free(map->indices);
        free(map->keys);
        free(map->values);

        map->indices = NULL;
        map->keys = NULL;
        map->values = NULL;
        map->size = 0;
        map->mod = 0;
    }
}

#endif /* UTOPIA_MAP_IMPL */

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_MAP_H */
