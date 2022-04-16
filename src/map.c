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
    map.key_bytes = key_size;
    map.value_bytes = value_size;
    map.keys = NULL;
    map.values = NULL;
    map.data_capacity = 0;
    map.element_count = 0;
    map.hash_func = &hash_uint;

    map.indexes = malloc((MAP_INITIAL_SIZE + 1) * sizeof(size_t*));
    for (size_t i = 0; i < MAP_INITIAL_SIZE; ++i) {
        map.indexes[i] = calloc(2, sizeof(size_t));
    }

    map.indexes[MAP_INITIAL_SIZE] = NULL;
    return map;
}

map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    map_t map;
    map.key_bytes = key_size;
    map.value_bytes = value_size;
    map.keys = NULL;
    map.values = NULL;
    map.data_capacity = 0;
    map.element_count = 0;
    map.hash_func = &hash_uint;

    map.indexes = malloc((reserve + 1) * sizeof(size_t*));
    for (size_t i = 0; i < reserve; ++i) {
        map.indexes[i] = calloc(2, sizeof(size_t));
    }
    map.indexes[reserve] = NULL;
    return map;
}

static inline size_t map_bucket_size(const size_t* bucket)
{
    size_t i;
    for (i = 0; bucket[i]; ++i) {}
    return i;
}

size_t map_capacity(const map_t* map)
{
    size_t i;
    for (i = 0; map->indexes[i]; ++i) {}
    return i;
}

size_t map_element_count(const map_t* map)
{
    return map->element_count;
}

void map_overload(map_t* map, size_t (*hash_func)(const void* key))
{
    map->hash_func = hash_func;
}

void* map_key_at(const map_t* map, const size_t index)
{
    return _map_key_at(map, index);
}

void* map_value_at(const map_t* map, const size_t index)
{
    return _map_value_at(map, index);
}

size_t map_search(const map_t* map, const void* key)
{
    const size_t capacity = map_capacity(map);
    const size_t hash = map->hash_func(key);

    size_t* bucket = map->indexes[hash % capacity];
    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash_func(k)) {
            return bucket[i];
        }
    }
    
    return 0;
}

size_t* map_search_all(const map_t* map, const void* key)
{
    const size_t capacity = map_capacity(map);
    const size_t hash = map->hash_func(key);

    size_t* bucket = map->indexes[hash % capacity];
    const size_t bucket_size = map_bucket_size(bucket);

    size_t indexes[bucket_size];
    size_t count = 0;

    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash_func(k)) {
            indexes[count++] = bucket[i];
        }
    }
    
    if (!count) {
        return NULL;
    }

    const size_t elements = count;
    size_t* ret = malloc((elements + 1) * sizeof(size_t));
    for (size_t i = 0; i < elements; ++i) {
        ret[i] = indexes[i];
    }
    ret[elements] = 0;
    
    return ret;
}

void map_resize(map_t* map, const size_t size)
{
    for (size_t i = 0; map->indexes[i]; ++i) {
        free(map->indexes[i]);
    }

    map->indexes = realloc(map->indexes, (size + 1) * sizeof(size_t*));
    for (size_t i = 0; i < size; ++i) {
        map->indexes[i] = calloc(2, sizeof(size_t));
    }
    map->indexes[size] = NULL;

    char* key = _map_key_at(map, 0);
    for (size_t i = 0; i < map->element_count; ++i) {
        
        const size_t hash = map->hash_func(key);
        size_t* bucket = map->indexes[hash % size];
        const size_t bucket_size = map_bucket_size(bucket);

        if (bucket_size) {
            map->indexes[hash % size] = realloc(bucket, bucket_size + 2);
            bucket = map->indexes[hash % size];
        }

        bucket[bucket_size] = i + 1;
        bucket[bucket_size + 1] = 0;

        key += map->key_bytes;
    }
}

void map_remove(map_t* restrict map, const void* restrict key)
{
    const size_t capacity = map_capacity(map);
    const size_t hash = map->hash_func(key);

    size_t index = 0;
    size_t* bucket = map->indexes[hash % capacity];
    
    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash_func(k)) {
            index = bucket[i];
            memmove(bucket + i, bucket + i + 1, map_bucket_size(bucket + i + 1) + 1);
            break;
        }
    }

    if (!index) {
        return;
    }

    const size_t search_index = index - 1;

    --map->element_count;
    
    char* k = _map_key_at(map, search_index);
    char* value = _map_value_at(map, search_index);
    
    memmove(k, k + map->key_bytes, (map->element_count - search_index) * map->key_bytes);
    memmove(value, value + map->value_bytes, (map->element_count - search_index) * map->value_bytes);
}

void map_push(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    if (map->element_count >= map->data_capacity) {
        map->data_capacity = map->data_capacity * 2 + !map->data_capacity;
        map->keys = !map->keys ? malloc(map->data_capacity * map->key_bytes) : realloc(map->keys, map->data_capacity * map->key_bytes);
        map->values = !map->values ? malloc(map->data_capacity * map->value_bytes) : realloc(map->values, map->data_capacity * map->value_bytes);
    }
    
    memcpy(_map_key_at(map, map->element_count), key, map->key_bytes);
    memcpy(_map_value_at(map, map->element_count), value, map->value_bytes);

    size_t capacity = map_capacity(map);
    if (map->element_count >= capacity) {
        capacity *= 2;
        map_resize(map, capacity);
    }

    const size_t hash = map->hash_func(key);
    size_t* bucket = map->indexes[hash % capacity];
    const size_t bucket_size = map_bucket_size(bucket);

    if (bucket_size) {
        map->indexes[hash % capacity] = realloc(bucket, bucket_size + 2);
        bucket = map->indexes[hash % capacity];
    }

    bucket[bucket_size] = ++map->element_count;
    bucket[bucket_size + 1] = 0;
}

size_t map_push_if(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    const size_t index = map_search(map, key);
    if (index) {
        return index;
    }

    size_t capacity = map_capacity(map);
    if (map->element_count >= capacity) {
        capacity *= 2;
        map_resize(map, capacity);
    }

    const size_t hash = map->hash_func(key);
    size_t* bucket = map->indexes[hash % capacity];
    
    const size_t bucket_size = map_bucket_size(bucket);
    if (bucket_size) {
        map->indexes[hash % capacity] = realloc(bucket, bucket_size + 2);
        bucket = map->indexes[hash % capacity];
    }

    bucket[bucket_size] = map->element_count;
    bucket[bucket_size + 1] = 0;

    if (map->element_count >= map->data_capacity) {
        map->data_capacity = map->data_capacity * 2 + !map->data_capacity;
        map->keys = !map->keys ? malloc(map->data_capacity * map->key_bytes) : realloc(map->keys, map->data_capacity * map->key_bytes);
        map->values = !map->values ? malloc(map->data_capacity * map->value_bytes) : realloc(map->values, map->data_capacity * map->value_bytes);
    }
    
    memcpy(_map_key_at(map, map->element_count), key, map->key_bytes);
    memcpy(_map_value_at(map, map->element_count), value, map->value_bytes);

    ++map->element_count;
    return 0;
}

void map_free(map_t* map)
{
    if (map->element_count) {
        free(map->keys);
        free(map->values);
    }

    for (size_t i = 0; map->indexes[i]; ++i) {
        free(map->indexes[i]);
    }
    free(map->indexes);
}