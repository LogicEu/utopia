
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

#ifndef UTOPIA_HASH_H
#define UTOPIA_HASH_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/*****************
Generic Hash Table
*****************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct hash {
    size_t** indices;
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
size_t hash_search(const struct hash* table, const void* data);
void hash_overload(struct hash* hash, size_t (*func)(const void*));
void* hash_index(const struct hash* table, const size_t index);
size_t hash_size(const struct hash* table);
size_t hash_capacity(const struct hash* table);
size_t hash_bytes(const struct hash* table);
void hash_resize(struct hash* table, const size_t new_size);
void* hash_push(struct hash* table, const void* data);
size_t hash_push_if(struct hash* table, const void* data);
int hash_remove(struct hash* table, const void* data);
void hash_free(struct hash* table);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_HASH_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_HASH_IMPLEMENTED
#define UTOPIA_HASH_IMPLEMENTED

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

static void bucket_remove(size_t** bucketref, const size_t index)
{
    size_t* bucket = *bucketref;
    const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
    size_t* ptr = bucket + index;
    memmove(ptr, ptr + 1, (size - index - 1) * sizeof(size_t));
    bucket[BUCKET_SIZE_INDEX] = size - 1 - BUCKET_DATA_INDEX;
    if (bucket[BUCKET_SIZE_INDEX] == 0) {
        free(bucket);
        *bucketref = NULL;
    }
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
                buckets[i][j] -= (buckets[i][j] >= removed);
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
    const unsigned char* str = *(unsigned char**)key;
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

/*****************
Generic Hash Table
*****************/

struct hash hash_create(const size_t bytes)
{
    struct hash table;
    table.indices = NULL;
    table.data = NULL;
    table.bytes = bytes + !bytes;
    table.size = 0;
    table.mod = 0;
    table.func = &hash_default;
    return table;
}

struct hash hash_reserve(const size_t bytes, const size_t reserve)
{
    struct hash table;
    table.bytes = bytes + !bytes;
    table.indices = reserve ? calloc(reserve, sizeof(size_t*)) : NULL;
    table.data = reserve ? malloc(reserve * table.bytes) : NULL;
    table.mod = reserve;
    table.size = 0;
    table.func = &hash_default;
    return table;
}

struct hash hash_copy(const struct hash* table)
{
    struct hash t = *table;
    if (table->mod) {
        size_t i, size;

        t.data = memdup(table->data, table->mod * table->bytes);
        t.indices = memdup(table->indices, table->mod * sizeof(size_t*));
        
        for (i = 0; i < table->mod; ++i) {
            size = BUCKET_SIZE(table->indices[i]);
            if (size) {
                size += BUCKET_DATA_INDEX;
                t.indices[i] = memdup(table->indices[i], size * sizeof(size_t));
            }
        }
    }
    return t;
}

size_t hash_capacity(const struct hash* table)
{
    return table->mod;
}

void hash_overload(struct hash* hash, size_t (*func)(const void*))
{
    hash->func = func;
}

void* hash_index(const struct hash* table, const size_t index)
{
    return _hash_index(table, index);
}

size_t hash_size(const struct hash* table)
{
    return table->size;
}

size_t hash_bytes(const struct hash* table)
{
    return table->bytes;
}

size_t hash_search(const struct hash* table, const void* data)
{
    if (table->mod) {
    
        size_t i;
        const size_t hash = table->func(data);
        const size_t* bucket = table->indices[hash % table->mod];
    
        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _hash_index(table, bucket[i]);
            if (hash == table->func(k)) {
                return bucket[i] + 1;
            }
        }
    }
    
    return 0;
}

void hash_resize(struct hash* table, const size_t new_size)
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
    table->indices = realloc(table->indices, table->mod * sizeof(size_t*));
    memset(table->indices, 0, table->mod * sizeof(size_t*));
    
    key = table->data;
    for (i = 0; i < size; ++i, key += bytes) {
        const size_t hash_mod = table->func(key) % table->mod;
        table->indices[hash_mod] = bucket_push(table->indices[hash_mod], i);
    }
}

int hash_remove(struct hash* table, const void* data)
{
    if (table->mod) {

        size_t i, search = 0;
        const size_t hash = table->func(data);
        size_t* bucket = table->indices[hash % table->mod];

        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _hash_index(table, bucket[i]);
            if (hash == table->func(k)) {
                search = i;
                break;
            }
        }

        if (search) {
            const size_t find = bucket[search];
            char* ptr = _hash_index(table, find);
            memmove(ptr, ptr + table->bytes, (--table->size - find) * table->bytes);
            bucket_remove(&bucket, search);
            buckets_reindex(table->indices, table->mod, find);
            return 1;
        }
    }

    return 0;
}

void* hash_push(struct hash* table, const void* data)
{
    void* ptr;
    size_t hashmod;
    if (table->size == table->mod) {
        hash_resize(table, table->mod * 2);
    }

    hashmod = table->func(data) % table->mod;
    ptr = _hash_index(table, table->size);
    table->indices[hashmod] = bucket_push(table->indices[hashmod], table->size++);
    memcpy(ptr, data, table->bytes);
    return ptr;
}

size_t hash_push_if(struct hash* table, const void* data)
{
    const size_t index = hash_search(table, data);
    if (index) {
        return index;
    }

    hash_push(table, data);
    return 0;
}

void hash_free(struct hash* table)
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

#endif /* UTOPIA_HASH_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
