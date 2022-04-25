#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/***********************************
 -> Generic <Key, Value> Hash Map <- 
***********************************/

#define MAP_INITIAL_SIZE 32
#define _map_bucket_at(map, i) (size_t*)((map)->indices[i] - 1)

static inline size_t idxcnt(const size_t* restrict indices)
{
    if (!indices) {
        return 0;
    }

    size_t i;
    for (i = 0; indices[i]; ++i) {}
    return i;
}

static inline size_t memcap(const size_t memlen)
{
    if (!memlen) {
        return 0;
    }

    size_t i;
    for (i = 1; i < memlen; ++i) {}
    return i;
}

static inline void* map_push_any(void* restrict data, const void* restrict val, const size_t bytes, const size_t size)
{
    const size_t len = size * bytes;
    const size_t cap = memcap(len);

    if (len + bytes > cap) {
        data = realloc(data, cap * 2);
    }

    memcpy((char*)data + size * bytes, val, bytes);
    return data;
}

static inline size_t* map_push_bucket(size_t* restrict bucket, const size_t index)
{
    const size_t cnt = !bucket ? 0 : idxcnt(bucket);
    
    if (!cnt) {
        bucket = malloc(sizeof(size_t) * 2);
    } else {
        const size_t ilen = (cnt + 1) * sizeof(size_t);
        const size_t icap = memcap(ilen);
        
        if (ilen + sizeof(size_t) > icap) {
            bucket = realloc(bucket, icap * 2);
        }
    }
    
    bucket[cnt] = index + 1;
    bucket[cnt + 1] = 0;

    return bucket;
}

static inline void map_push_index(map_t* restrict map, const size_t mod_hash, const size_t index)
{
    size_t* bucket = map_push_bucket(_map_bucket_at(map, mod_hash), index);
    map->indices[mod_hash] = (size_t)bucket + 1;
}

static inline void* map_remove_any(void* restrict data, const size_t bytes, const size_t size, const size_t index)
{
    char* ptr = (char*)data + index * bytes;
    memmove(ptr, ptr + bytes, (size - index) * bytes);

    const size_t len = bytes * size;
    const size_t half_cap = memcap(len) >> 1;

    if (!len) {
        free(data);
        data = NULL;
    }
    else if (len - bytes <= half_cap) {
        data = realloc(data, half_cap);
    }

    return data;
}

static inline size_t* map_remove_bucket(size_t* restrict bucket, const size_t index)
{
    const size_t size = idxcnt(bucket);
    const size_t len = (size + 1) * sizeof(size_t);

    if (len == sizeof(size_t)) {
        free(bucket);
        bucket = NULL;
    } else {
        size_t* ptr = bucket + index;
        memmove(ptr, ptr + 1, (size - index) * sizeof(size_t));

        const size_t half_cap = memcap(len) >> 1;
        if (len - sizeof(size_t) <= half_cap) {
            bucket = realloc(bucket, half_cap);
        }
    }

    return bucket;
}

static inline void map_bucket_init(size_t* restrict indices, const size_t n)
{
    const size_t m = n - 1;
    for (size_t i = 0; i < m; ++i) {
        indices[i] = 1;
    }
    indices[m] = 0;
}

static inline void map_bucket_free(map_t* restrict map)
{
    for (size_t i = 0; map->indices[i]; ++i) {
        size_t* bucket = _map_bucket_at(map, i);
        if (bucket) {
            free(bucket);
        }
    }
}

map_t map_create(const size_t key_size, const size_t value_size)
{
    map_t map;
    map.indices = NULL;
    map.keys = NULL;
    map.values = NULL;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.hash = &hash_cstr;
    return map;
}

map_t map_reserve(const size_t key_size, const size_t value_size, const size_t reserve)
{
    map_t map;
    
    const size_t r = reserve + !reserve * 2;
    map.indices = malloc(memcap(r * sizeof(size_t)));
    map_bucket_init(map.indices, r);

    map.keys = NULL;
    map.values = NULL;
    map.key_bytes = key_size + !key_size;
    map.value_bytes = value_size + !value_size;
    map.hash = &hash_uint;
    return map;
}

size_t map_bucket_size(const size_t* bucket)
{
    return idxcnt(bucket);
}

size_t map_bucket_count(const map_t* map)
{
    return idxcnt(map->indices);
}

size_t map_capacity(const map_t* map)
{
    return memcap(map_size(map) * map->key_bytes) / map->key_bytes;
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
    if (!map->indices) {
        return 0;
    }

    size_t size = 0;
    for (size_t i = 0; map->indices[i]; ++i) {
        size_t* bucket = _map_bucket_at(map, i);
        if (bucket) {
            for (size_t j = 0; bucket[j]; ++j) {
                ++size;
            }
        }
    }
    return size;
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
    const size_t mod = idxcnt(map->indices);
    const size_t hash = map->hash(key);

    size_t* bucket = _map_bucket_at(map, hash % mod);
    if (!bucket) {
        return 0;
    }

    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash(k)) {
            return bucket[i];
        }
    }
    
    return 0;
}

size_t* map_search_all(const map_t* map, const void* key)
{
    const size_t mod = idxcnt(map->indices);
    const size_t hash = map->hash(key);

    size_t* bucket = _map_bucket_at(map, hash % mod);
    if (!bucket) {
        return NULL;
    }

    const size_t bucket_size = idxcnt(bucket);
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
    memcpy(ret, indices, elements * sizeof(size_t));
    ret[elements] = 0;
    
    return ret;
}

size_t* map_bucket_at(const map_t* restrict map, const size_t index)
{
    return _map_bucket_at(map, index);
}

void map_resize(map_t* restrict map, const size_t new_size)
{
    const size_t ksize = map_size(map);
    const size_t bytes = map->key_bytes;

    map_bucket_free(map);
    map->indices = realloc(map->indices, memcap(new_size * sizeof(size_t)));
    map_bucket_init(map->indices, new_size);
    
    const char* key = map->keys;
    for (size_t i = 0; i < ksize; ++i, key += bytes) {
        map_push_index(map, map->hash(key) % (new_size - 1), i);
    }
}

void map_remove(map_t* restrict map, const void* restrict key)
{
    const size_t mod = idxcnt(map->indices);
    const size_t hash = map->hash(key);
    const size_t index = hash % mod;

    size_t* bucket = _map_bucket_at(map, index);
    if (!bucket) {
        return;
    }
    
    size_t search = 0;
    for (size_t i = 0; bucket[i]; ++i) {
        void* k = _map_key_at(map, bucket[i] - 1);
        if (hash == map->hash(k)) {
            search = i + 1;
            break;
        }
    }

    if (!search) {
        return;
    }

    const size_t find = bucket[search - 1] - 1;
    bucket = map_remove_bucket(bucket, search - 1);

    for (size_t i = 0; map->indices[i]; ++i) {
        size_t* buck = _map_bucket_at(map, i);
        if (buck) {
            for (size_t j = 0; buck[j]; j++) {
                buck[j] -= (buck[j] >= find);
            }
        }
    }

    map->indices[index] = (size_t)bucket + 1;
    const size_t size = map_size(map);
    
    map->keys = map_remove_any(map->keys, map->key_bytes, size, find);
    map->values = map_remove_any(map->values, map->value_bytes, size, find);
}

void map_push(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    const size_t size = map_size(map);

    if (!size) {
        
        map->indices = malloc(sizeof(size_t) * MAP_INITIAL_SIZE);
        map_bucket_init(map->indices, MAP_INITIAL_SIZE);
        map->keys = malloc(memcap(map->key_bytes));
        map->values = malloc(memcap(map->value_bytes));
    
    } 

    map->keys = map_push_any(map->keys, key, map->key_bytes, size);
    map->values = map_push_any(map->values, value, map->value_bytes, size);

    size_t mod = idxcnt(map->indices);
    if (size + 1 > mod) {
        mod = (mod + 1) * 2;
        map_resize(map, mod--);
    }

    map_push_index(map, map->hash(key) % mod, size);
}

size_t map_push_if(map_t* restrict map, const void* restrict key, const void* restrict value)
{
    const size_t index = map_search(map, key);
    if (index) {
        return index;
    }

    map_push(map, key, value);
    return 0;
}

void map_free(map_t* restrict map)
{
    if (!map->indices) {
        return;
    }

    map_bucket_free(map);
    free(map->indices);
    free(map->keys);
    free(map->values);

    map->indices = NULL;
    map->keys = NULL;
    map->values = NULL;
}