
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

#ifndef UTOPIA_SET_H
#define UTOPIA_SET_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/*****************
 Generic Hash Set
*****************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct set {
    size_t** indices;
    void* data;
    size_t bytes;
    size_t size;
    size_t mod;
    size_t (*func)(const void*);
};

#define _set_index(set, i) ((char*)(set)->data + (i) * (set)->bytes)

struct set set_create(const size_t bytes);
struct set set_reserve(const size_t bytes, const size_t reserve);
struct set set_copy(const struct set* set);
size_t set_search(const struct set* set, const void* data);
void set_overload(struct set* hash, size_t (*func)(const void*));
void* set_index(const struct set* set, const size_t index);
size_t set_size(const struct set* set);
size_t set_capacity(const struct set* set);
size_t set_bytes(const struct set* set);
void set_resize(struct set* set, const size_t new_size);
void* set_push(struct set* set, const void* data);
size_t set_push_if(struct set* set, const void* data);
int set_remove(struct set* set, const void* data);
void set_free(struct set* set);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_SET_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_SET_IMPLEMENTED
#define UTOPIA_SET_IMPLEMENTED

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

static size_t set_default(const void* key)
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
 Generic Hash Set
*****************/

struct set set_create(const size_t bytes)
{
    struct set set;
    set.indices = NULL;
    set.data = NULL;
    set.bytes = bytes + !bytes;
    set.size = 0;
    set.mod = 0;
    set.func = &set_default;
    return set;
}

struct set set_reserve(const size_t bytes, const size_t reserve)
{
    struct set set;
    set.bytes = bytes + !bytes;
    set.indices = reserve ? calloc(reserve, sizeof(size_t*)) : NULL;
    set.data = reserve ? malloc(reserve * set.bytes) : NULL;
    set.mod = reserve;
    set.size = 0;
    set.func = &set_default;
    return set;
}

struct set set_copy(const struct set* set)
{
    struct set t = *set;
    if (set->mod) {
        size_t i, size;

        t.data = memdup(set->data, set->mod * set->bytes);
        t.indices = memdup(set->indices, set->mod * sizeof(size_t*));
        
        for (i = 0; i < set->mod; ++i) {
            size = BUCKET_SIZE(set->indices[i]);
            if (size) {
                size += BUCKET_DATA_INDEX;
                t.indices[i] = memdup(set->indices[i], size * sizeof(size_t));
            }
        }
    }
    return t;
}

size_t set_capacity(const struct set* set)
{
    return set->mod;
}

void set_overload(struct set* hash, size_t (*func)(const void*))
{
    hash->func = func;
}

void* set_index(const struct set* set, const size_t index)
{
    return _set_index(set, index);
}

size_t set_size(const struct set* set)
{
    return set->size;
}

size_t set_bytes(const struct set* set)
{
    return set->bytes;
}

size_t set_search(const struct set* set, const void* data)
{
    if (set->mod) {
    
        size_t i;
        const size_t hash = set->func(data);
        const size_t* bucket = set->indices[hash % set->mod];
    
        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _set_index(set, bucket[i]);
            if (hash == set->func(k)) {
                return bucket[i] + 1;
            }
        }
    }
    
    return 0;
}

void set_resize(struct set* set, const size_t new_size)
{
    size_t i;
    const char* key;
    const size_t size = set->size;
    const size_t bytes = set->bytes;

    if (set->mod) {
        buckets_free(set->indices, set->mod);
    }

    set->mod = new_size + !new_size * UTOPIA_HASH_SIZE;
    set->data = realloc(set->data, set->mod * set->bytes);
    set->indices = realloc(set->indices, set->mod * sizeof(size_t*));
    memset(set->indices, 0, set->mod * sizeof(size_t*));
    
    key = set->data;
    for (i = 0; i < size; ++i, key += bytes) {
        const size_t set_mod = set->func(key) % set->mod;
        set->indices[set_mod] = bucket_push(set->indices[set_mod], i);
    }
}

int set_remove(struct set* set, const void* data)
{
    if (set->mod) {

        size_t i, search = 0;
        const size_t hash = set->func(data);
        size_t* bucket = set->indices[hash % set->mod];

        const size_t size = BUCKET_SIZE(bucket) + BUCKET_DATA_INDEX;
        for (i = BUCKET_DATA_INDEX; i < size; ++i) {
            void* k = _set_index(set, bucket[i]);
            if (hash == set->func(k)) {
                search = i;
                break;
            }
        }

        if (search) {
            const size_t find = bucket[search];
            char* ptr = _set_index(set, find);
            memmove(ptr, ptr + set->bytes, (--set->size - find) * set->bytes);
            bucket_remove(&bucket, search);
            buckets_reindex(set->indices, set->mod, find);
            return 1;
        }
    }

    return 0;
}

void* set_push(struct set* set, const void* data)
{
    void* ptr;
    size_t hashmod;
    if (set->size == set->mod) {
        set_resize(set, set->mod * 2);
    }

    hashmod = set->func(data) % set->mod;
    ptr = _set_index(set, set->size);
    set->indices[hashmod] = bucket_push(set->indices[hashmod], set->size++);
    memcpy(ptr, data, set->bytes);
    return ptr;
}

size_t set_push_if(struct set* set, const void* data)
{
    const size_t index = set_search(set, data);
    if (index) {
        return index;
    }

    set_push(set, data);
    return 0;
}

void set_free(struct set* set)
{
    if (set->indices) {
        buckets_free(set->indices, set->mod);
        free(set->indices);
        free(set->data);

        set->indices = NULL;
        set->data = NULL;
        set->size = 0;
        set->mod = 0;
    }
}

#endif /* UTOPIA_SET_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
