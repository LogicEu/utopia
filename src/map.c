#include <utopia.h>
#include USTDLIB_H
#include USTRING_H
#include "bucket.h"

/***********************************
 -> Generic <Key, Value> Hash Map <- 
***********************************/

static void* memdup(const void* src, size_t size)
{
    void* dup = malloc(size);
    memcpy(dup, src, size);
    return dup;
}

map_t map_create(const size_t key_size, const size_t value_size)
{
    map_t map;
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

map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    map_t map;
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

map_t map_copy(const map_t* map)
{
    map_t m = *map;
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

size_t map_capacity(const map_t* map)
{
    return map->mod;
}

void map_overload(map_t* map, size_t (*func)(const void*))
{
    map->func = func;
}

void* map_key_at(const map_t* map, const size_t index)
{
    return _map_key_at(map, index);
}

void* map_value_at(const map_t* map, const size_t index)
{
    return _map_value_at(map, index);
}

size_t map_size(const map_t*map)
{
    return map->size;
}

size_t map_key_bytes(const map_t* map)
{
    return map->key_bytes;
}

size_t map_value_bytes(const map_t* map)
{
    return map->value_bytes;
}

index_t map_search(const map_t* map, const void* data)
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

index_t* map_search_all(const map_t*map, const void*key)
{
    index_t* found = NULL;
    
    if (map->mod) {

        size_t i, count = 0;
        const size_t hash = map->func(key);
        bucket_t bucket = map->indices[hash % map->mod];

        const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
        index_t indices[size - BUCKET_DATA_INDEX];

        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _map_key_at(map, bucket[i]);
            if (hash == map->func(k)) {
                indices[count++] = bucket[i] + 1;
            }
        }

        if (count) {
            const size_t elements = count;
            found = malloc((elements + 1) * sizeof(index_t));
            memcpy(found, indices, elements * sizeof(index_t));
            found[elements] = 0;
        }
    }

    return found;
}

void map_resize(map_t*map, const size_t new_size)
{
    size_t i;
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
    
    const char* key = map->keys;
    for (i = 0; i < size; ++i, key += bytes) {
        const size_t hash_mod = map->func(key) % map->mod;
        map->indices[hash_mod] = bucket_push(map->indices[hash_mod], i);
    }
}

void map_remove(map_t*map, const void*key)
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

void map_push(map_t*map, const void*key, const void*value)
{
    if (map->size == map->mod) {
        map_resize(map, map->mod * 2);
    }

    const size_t hash_mod = map->func(key) % map->mod;
    map->indices[hash_mod] = bucket_push(map->indices[hash_mod], map->size);

    memcpy(_map_key_at(map, map->size), key, map->key_bytes);
    memcpy(_map_value_at(map, map->size), value, map->value_bytes);
    ++map->size;
}

size_t map_push_if(map_t*map, const void*key, const void*value)
{
    const size_t index = map_search(map, key);
    if (index) {
        return index;
    }

    map_push(map, key, value);
    return 0;
}

void map_free(map_t*map)
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
