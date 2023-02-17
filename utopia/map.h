
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

#ifndef UTOPIA_MAP_H
#define UTOPIA_MAP_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/****************************
Generic <Key, Value> Hash Map
*****************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct map {
    size_t** indices;
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
size_t map_search(const struct map* map, const void* key);
void map_overload(struct map* map, size_t (*hash_func)(const void*));
void* map_key_at(const struct map* map, const size_t index);
void* map_value_at(const struct map* map, const size_t index);
size_t map_size(const struct map* map);
size_t map_capacity(const struct map* map);
size_t map_key_bytes(const struct map* map);
size_t map_value_bytes(const struct map* map);
void map_resize(struct map* map, const size_t size);
void* map_push(struct map* map, const void* key, const void* value);
size_t map_push_if(struct map* map, const void* key, const void* value);
void map_remove(struct map* map, const void* key);
void map_free(struct map* map);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_MAP_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_MAP_IMPLEMENTED
#define UTOPIA_MAP_IMPLEMENTED

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

/* Bucket Implementation */

#ifndef UTOPIA_BUCKET_IMPLEMENTED
#define UTOPIA_BUCKET_IMPLEMENTED

#define BUCKET_CAP_INDEX 0
#define BUCKET_SIZE_INDEX 1
#define BUCKET_DATA_INDEX 2

#define BUCKET_SIZE(bucket) (bucket ? bucket[BUCKET_SIZE_INDEX] : 0)
#define BUCKET_CAP(bucket) (bucket ? bucket[BUCKET_CAP_INDEX] : 0)
#define BUCKET_DATA(bucket) (bucket ? bucket[BUCKET_DATA_INDEX] : 0)

static size_t* bucket_push(size_t* bucket, size_t index)
{
    size_t size = BUCKET_SIZE(bucket);
    size_t cap = BUCKET_CAP(bucket);

    if (size >= cap) {
        cap = (!cap + cap) * 2;
        bucket = realloc(bucket, (cap + BUCKET_DATA_INDEX) * sizeof(size_t));
        bucket[BUCKET_CAP_INDEX] = cap;
    }

    bucket[BUCKET_DATA_INDEX + size] = index;
    bucket[BUCKET_SIZE_INDEX] = size + 1;

    return bucket;
}

static void bucket_remove(size_t* bucket, const size_t index)
{
    const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
    size_t* ptr = bucket + index;
    memmove(ptr, ptr + 1, (size - index - 1) * sizeof(size_t));
    bucket[BUCKET_SIZE_INDEX] = size - 1 - BUCKET_DATA_INDEX;
}

#endif /* UTOPIA_BUCKET_IMPLEMENTED */

/* Bucket Array Implementation */

#ifndef UTOPIA_BUCKET_ARRAY_IMPLEMENTED
#define UTOPIA_BUCKET_ARRAY_IMPLEMENTED

static void buckets_reindex(size_t** buckets, const size_t size, const size_t removed)
{
    size_t i, j;
    for (i = 0; i < size; ++i) {
        if (buckets[i]) {
            const size_t count = BUCKET_SIZE(buckets[i]) + BUCKET_DATA_INDEX;
            for (j = BUCKET_DATA_INDEX; j < count; j++) {
                buckets[i][j] -= (buckets[i][i] >= removed);
            }
        }
    }
}

static void buckets_free(size_t** buckets, const size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        if (buckets[i]) {
            free(buckets[i]);
        }
    }
}

#endif /* UTOPIA_BUCKET_ARRAY_IMPLEMENTED */

/* Hashable Implementation */

#ifndef UTOPIA_HASHABLE_IMPLEMENTED
#define UTOPIA_HASHABLE_IMPLEMENTED

#ifndef UTOPIA_HASH_SIZE
#define UTOPIA_HASH_SIZE 32
#endif

static void* memdup(const void* src, size_t size)
{
    void* dup = malloc(size);
    memcpy(dup, src, size);
    return dup;
}

static size_t hash_default(const void* key)
{
#ifndef UTOPIA_HASH_UINT
    int c;
    size_t hash = 5381;
    const unsigned char* str = key;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
#else
    size_t x = *(size_t*)key;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    return (x >> 16) ^ x;
#endif
}

#endif /* UTOPIA_HASHABLE_IMPLEMENTED */

/****************************
Generic <Key, Value> Hash Map
*****************************/

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
    map.func = &hash_default;
    return map;
}

struct map map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    struct map map;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.indices = reserve ? calloc(reserve, sizeof(size_t*)) : NULL;
    map.keys = reserve ? malloc(reserve * map.key_bytes) : NULL;
    map.values = reserve ? malloc(reserve * map.value_bytes) : NULL;
    map.mod = reserve;
    map.size = 0;
    map.func = &hash_default;
    return map;
}

struct map map_copy(const struct map* map)
{
    struct map m = *map;
    if (map->mod) {
        size_t i, size;

        m.keys = memdup(map->keys, map->mod * map->key_bytes);
        m.values = memdup(map->values, map->mod * map->value_bytes);
        m.indices = memdup(map->indices, map->mod * sizeof(size_t*));
        
        for (i = 0; i < map->mod; ++i) {
            size = BUCKET_SIZE(map->indices[i]);
            if (size) {
                size += BUCKET_DATA_INDEX;
                m.indices[i] = memdup(map->indices[i], size * sizeof(size_t));
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

size_t map_search(const struct map* map, const void* data)
{
    if (map->mod) {
    
        size_t i;
        const size_t hash = map->func(data);
        size_t* bucket = map->indices[hash % map->mod];
    
        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
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
    map->indices = realloc(map->indices, map->mod * sizeof(size_t*));
    memset(map->indices, 0, map->mod * sizeof(size_t*));
    
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
        size_t* bucket = map->indices[hash % map->mod];      

        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _map_key_at(map, bucket[i]);
            if (hash == map->func(k)) {
                search = i;
                break;
            }
        }
        
        if (search) {
            const size_t find = bucket[search];
            
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

void* map_push(struct map* map, const void* key, const void* value)
{
    void* ptr;
    size_t hashmod;
    if (map->size == map->mod) {
        map_resize(map, map->mod * 2);
    }

    hashmod = map->func(key) % map->mod;
    map->indices[hashmod] = bucket_push(map->indices[hashmod], map->size);

    ptr = _map_value_at(map, map->size);
    memcpy(_map_key_at(map, map->size), key, map->key_bytes);
    memcpy(ptr, value, map->value_bytes);
    ++map->size;
    return ptr;
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

#endif /* UTOPIA_MAP_IMPLEMENTATED */
#endif /* UTOPIA_IMPLEMENTATION */
