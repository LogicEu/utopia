#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/***********************************
 -> Generic <Key, Value> Hash Map <- 
***********************************/

#define MAP_INITIAL_SIZE 255

map_t map_create(const size_t key_size, const size_t value_size)
{
    map_t map;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.keys = NULL;
    map.values = NULL;
    map.capacity = 0;
    map.size = 0;
    map.hash = &hash_cstr;

    map.indices = malloc((MAP_INITIAL_SIZE + 1) * sizeof(size_t*));
    for (size_t i = 0; i < MAP_INITIAL_SIZE; ++i) {
        map.indices[i] = calloc(2, sizeof(size_t));
    }

    map.indices[MAP_INITIAL_SIZE] = NULL;
    return map;
}

map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    map_t map;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.keys = NULL;
    map.values = NULL;
    map.capacity = 0;
    map.size = 0;
    map.hash = &hash_uint;

    map.indices = malloc((reserve + 1) * sizeof(size_t*));
    for (size_t i = 0; i < reserve; ++i) {
        map.indices[i] = calloc(2, sizeof(size_t));
    }
    map.indices[reserve] = NULL;
    return map;
}

size_t map_bucket_size(const size_t* bucket)
{
    size_t i;
    for (i = 0; bucket[i]; ++i) {}
    return i;
}

size_t map_bucket_count(const map_t* map)
{
    size_t i;
    for (i = 0; map->indices[i]; ++i) {}
    return i;
}

size_t map_capacity(const map_t* map)
{
    return map->capacity;
}

void map_overload(map_t* map, size_t (*func)(const void* key))
{
    map->hash = func;
}

void* map_key_at(const map_t* map, const size_t index)
{
    return _map_key_at(map, index);
}

void* map_value_at(const map_t* map, const size_t index)
{
    return _map_value_at(map, index);
}

size_t map_size(const map_t* map)
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

size_t map_search(const map_t* map, const void* key)
{
    const size_t mod = map_bucket_count(map);
    const size_t hash = map->hash(key);

    size_t* bucket = map->indices[hash % mod];
    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash(k)) {
            return bucket[i];
        }
    }
    
    return 0;
}

size_t* map_bucket_at(const map_t* map, const size_t index)
{
    return map->indices[index];
}

size_t* map_search_all(const map_t* map, const void* key)
{
    const size_t mod = map_bucket_count(map);
    const size_t hash = map->hash(key);

    size_t* bucket = map->indices[hash % mod];
    const size_t bucket_size = map_bucket_size(bucket);

    size_t indices[bucket_size];
    size_t count = 0;

    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash(k)) {
            indices[count++] = bucket[i];
        }
    }
    
    if (!count) {
        return NULL;
    }

    const size_t elements = count;
    size_t* ret = malloc((elements + 1) * sizeof(size_t));
    for (size_t i = 0; i < elements; ++i) {
        ret[i] = indices[i];
    }
    ret[elements] = 0;
    
    return ret;
}

void map_resize(map_t* map, const size_t size)
{
    for (size_t i = 0; map->indices[i]; ++i) {
        free(map->indices[i]);
    }

    map->indices = realloc(map->indices, (size + 1) * sizeof(size_t*));
    for (size_t i = 0; i < size; ++i) {
        map->indices[i] = calloc(2, sizeof(size_t));
    }
    map->indices[size] = NULL;

    char* key = _map_key_at(map, 0);
    for (size_t i = 0; i < map->size; ++i) {
        
        const size_t hash = map->hash(key);
        size_t* bucket = map->indices[hash % size];
        const size_t bucket_size = map_bucket_size(bucket);

        if (bucket_size) {
            map->indices[hash % size] = realloc(bucket, bucket_size + 2);
            bucket = map->indices[hash % size];
        }

        bucket[bucket_size] = i + 1;
        bucket[bucket_size + 1] = 0;

        key += map->key_bytes;
    }
}

void map_remove(map_t* restrict map, const void* restrict key)
{
    const size_t mod = map_bucket_count(map);
    const size_t hash = map->hash(key);

    size_t index = 0;
    size_t* bucket = map->indices[hash % mod];
    
    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash(k)) {
            index = bucket[i];
            memmove(bucket + i, bucket + i + 1, map_bucket_size(bucket + i + 1) + 1);
            break;
        }
    }

    if (!index) {
        return;
    }

    const size_t search_index = index - 1;

    --map->size;
    
    char* k = _map_key_at(map, search_index);
    char* value = _map_value_at(map, search_index);
    
    memmove(k, k + map->key_bytes, (map->size - search_index) * map->key_bytes);
    memmove(value, value + map->value_bytes, (map->size - search_index) * map->value_bytes);
}

void map_push(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    if (map->size >= map->capacity) {
        map->capacity = map->capacity * 2 + !map->capacity;
        map->keys = !map->keys ? malloc(map->capacity * map->key_bytes) : realloc(map->keys, map->capacity * map->key_bytes);
        map->values = !map->values ? malloc(map->capacity * map->value_bytes) : realloc(map->values, map->capacity * map->value_bytes);
    }
    
    memcpy(_map_key_at(map, map->size), key, map->key_bytes);
    memcpy(_map_value_at(map, map->size), value, map->value_bytes);

    size_t mod = map_bucket_count(map);
    if (map->size >= mod) {
        mod *= 2;
        map_resize(map, mod);
    }

    const size_t hash = map->hash(key);
    size_t* bucket = map->indices[hash % mod];
    const size_t bucket_size = map_bucket_size(bucket);

    if (bucket_size) {
        map->indices[hash % mod] = realloc(bucket, bucket_size + 2);
        bucket = map->indices[hash % mod];
    }

    bucket[bucket_size] = ++map->size;
    bucket[bucket_size + 1] = 0;
}

size_t map_push_if(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    const size_t index = map_search(map, key);
    if (index) {
        return index;
    }

    size_t mod = map_bucket_count(map);
    if (map->size >= mod) {
        mod *= 2;
        map_resize(map, mod);
    }

    const size_t hash = map->hash(key);
    size_t* bucket = map->indices[hash % mod];
    
    const size_t bucket_size = map_bucket_size(bucket);
    if (bucket_size) {
        map->indices[hash % mod] = realloc(bucket, bucket_size + 2);
        bucket = map->indices[hash % mod];
    }

    bucket[bucket_size] = map->size;
    bucket[bucket_size + 1] = 0;

    if (map->size >= map->capacity) {
        map->capacity = map->capacity * 2 + !map->capacity;
        map->keys = !map->keys ? malloc(map->capacity * map->key_bytes) : realloc(map->keys, map->capacity * map->key_bytes);
        map->values = !map->values ? malloc(map->capacity * map->value_bytes) : realloc(map->values, map->capacity * map->value_bytes);
    }
    
    memcpy(_map_key_at(map, map->size), key, map->key_bytes);
    memcpy(_map_value_at(map, map->size), value, map->value_bytes);

    ++map->size;
    return 0;
}

void map_free(map_t* map)
{
    if (map->size) {
        free(map->keys);
        free(map->values);
    }

    for (size_t i = 0; map->indices[i]; ++i) {
        free(map->indices[i]);
    }
    free(map->indices);
}