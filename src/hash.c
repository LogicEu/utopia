#include <utopia/hash.h>
#include USTDLIB_H
#include USTRING_H
#include "bucket.h"

/********************************
 -> Generic Hash Indexed Table <- 
********************************/

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

hash_t hash_create(const size_t bytes)
{
    hash_t table;
    table.indices = NULL;
    table.data = NULL;
    table.bytes = bytes + !bytes;
    table.size = 0;
    table.mod = 0;
    table.func = &hash_cstr;
    return table;
}

hash_t hash_reserve(const size_t bytes, const size_t reserve)
{
    hash_t table;
    table.bytes = bytes + !bytes;
    table.indices = reserve ? calloc(reserve, sizeof(bucket_t)) : NULL;
    table.data = reserve ? malloc(reserve * table.bytes) : NULL;
    table.mod = reserve;
    table.size = 0;
    table.func = &hash_cstr;
    return table;
}

hash_t hash_copy(const hash_t* table)
{
    hash_t t = *table;
    if (table->mod) {
        size_t i, size;

        t.data = memdup(table->data, table->mod * table->bytes);
        t.indices = memdup(table->indices, table->mod * sizeof(bucket_t));
        
        for (i = 0; i < table->mod; ++i) {
            size = bucket_size(table->indices[i]);
            if (size) {
                size += BUCKET_DATA_INDEX;
                t.indices[i] = memdup(table->indices[i], size * sizeof(index_t));
            }
        }
    }
    return t;
}

size_t hash_capacity(const hash_t* table)
{
    return table->mod;
}

void hash_overload(hash_t* hash, size_t (*func)(const void*))
{
    hash->func = func;
}

void* hash_index(const hash_t* table, const size_t index)
{
    return _hash_index(table, index);
}

size_t hash_size(const hash_t* table)
{
    return table->size;
}

size_t hash_bytes(const hash_t* table)
{
    return table->bytes;
}

index_t hash_search(const hash_t* table, const void* data)
{
    if (table->mod) {
    
        size_t i;
        const size_t hash = table->func(data);
        bucket_t bucket = table->indices[hash % table->mod];
    
        const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _hash_index(table, bucket[i]);
            if (hash == table->func(k)) {
                return bucket[i] + 1;
            }
        }
    }
    
    return 0;
}

void hash_resize(hash_t* table, const size_t new_size)
{
    size_t i;
    const char* key;
    const size_t size = table->size;
    const size_t bytes = table->bytes;

    if (table->mod) {
        buckets_free(table->indices, table->mod);
    }

    table->mod = new_size + !new_size * UTOPIA_HASH_SIZE;
    table->data = realloc(table->data, table->mod * table->bytes);
    table->indices = realloc(table->indices, table->mod * sizeof(bucket_t));
    memset(table->indices, 0, table->mod * sizeof(bucket_t));
    
    key = table->data;
    for (i = 0; i < size; ++i, key += bytes) {
        const size_t hash_mod = table->func(key) % table->mod;
        table->indices[hash_mod] = bucket_push(table->indices[hash_mod], i);
    }
}

void hash_remove(hash_t* table, const void* data)
{
    if (table->mod) {

        size_t i, search = 0;
        const size_t hash = table->func(data);
        bucket_t bucket = table->indices[hash % table->mod];

        const size_t size = bucket_size(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _hash_index(table, bucket[i]);
            if (hash == table->func(k)) {
                search = i;
                break;
            }
        }

        if (search) {
            const index_t find = bucket[search];
            char* ptr = _hash_index(table, find);
            memmove(ptr, ptr + table->bytes, (--table->size - find) * table->bytes);
            bucket_remove(bucket, search);
            buckets_reindex(table->indices, table->mod, find);
        }
    }
}

void hash_push(hash_t* table, const void* data)
{
    size_t hashmod;
    if (table->size == table->mod) {
        hash_resize(table, table->mod * 2);
    }

    hashmod = table->func(data) % table->mod;
    table->indices[hashmod] = bucket_push(table->indices[hashmod], table->size);
    
    memcpy(_hash_index(table, table->size++), data, table->bytes);
}

index_t hash_push_if(hash_t* table, const void* data)
{
    const index_t index = hash_search(table, data);
    if (index) {
        return index;
    }

    hash_push(table, data);
    return 0;
}

void hash_free(hash_t* table)
{
    if (table->indices) {
        buckets_free(table->indices, table->mod);
        free(table->indices);
        free(table->data);

        table->indices = NULL;
        table->data = NULL;
        table->size = 0;
        table->mod = 0;
    }
}
