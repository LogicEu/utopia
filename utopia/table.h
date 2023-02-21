
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

#ifndef UTOPIA_TABLE_H
#define UTOPIA_TABLE_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/**********************
Unordered Indexed Table
***********************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct table {
    size_t* indices;
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
};

#define _table_at(table, i) (((char*)(table)->data) + (table)->bytes * i)
#define _table_index_at(table, i) ((table)->indices[i + 2])
#define _table_value_at(table, i) (_table_at((table), _table_index_at(table, i)))

struct table table_create(const size_t bytes);
size_t table_push(struct table* table, const void* data);
void table_push_index(struct table* table, const size_t index);
void table_push_data(struct table* table, const void* data);
void table_remove(struct table* table, const size_t index);
struct table table_compress(const void* data, const size_t bytes, const size_t count);
void* table_decompress(const struct table* table, size_t* size);
void* table_values(const struct table* table);
void* table_value_at(const struct table* table, const size_t index);
size_t* table_indices(const struct table* table);
size_t table_index_at(const struct table* table, const size_t index);
size_t table_indices_size(const struct table* table);
size_t table_values_size(const struct table* table);
size_t table_bytes(const struct table* table);
void table_free(struct table* table);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_TABLE_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_TABLE_IMPLEMENTATION
#define UTOPIA_TABLE_IMPLEMENTATION

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

/**********************
Unordered Indexed Table
***********************/

struct table table_create(const size_t bytes)
{
    struct table table;
    table.indices = NULL;
    table.data = NULL;
    table.bytes = bytes + !bytes;
    table.size = 0;
    table.capacity = 0;
    return table;
}

size_t table_search(const struct table* table, const void* data)
{
    size_t i;
    const size_t bytes = table->bytes, count = table->size;
    const char* ptr = table->data;
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    return 0;
}

void table_push_index(struct table* table, const size_t index)
{
    table->indices = bucket_push(table->indices, index);
}

void table_push_data(struct table* table, const void* data)
{ 
    if (table->size == table->capacity) {
        table->capacity = table->capacity * 2 + !table->capacity;
        table->data = realloc(table->data, table->capacity * table->bytes);
    }
    memcpy(_table_at(table, table->size++), data, table->bytes);
}

size_t table_push(struct table* table, const void* data)
{
    size_t search = table_search(table, data); 
    if (!search) {
        table_push_data(table, data);
        search = table->size;
    } 

    table_push_index(table, search - 1);
    return search;
}

void table_remove(struct table* table, const size_t index)
{
    if (table->indices) {
        size_t size;
        size_t* indices, i;
        char* ptr = _table_at(table, index);
        memmove(ptr, ptr + table->bytes, (--table->size - index) * table->bytes);
        
        indices = table_indices(table);
        size = table_indices_size(table);
        for (i = 0; i < size; ++i) {
            if (indices[i] == index) {
                bucket_remove(table->indices, (size_t)i);
            } 
            else if (table->indices[i] > index) {
                --table->indices[i];
            }
        }
    }
}

struct table table_compress(const void* data, const size_t bytes, const size_t count)
{   
    size_t i;
    const char* ptr = data;
    struct table table = table_create(bytes);

    for (i = 0; i < count; ++i) {
        table_push(&table, ptr);
        ptr += bytes;
    }

    return table;
}

void* table_decompress(const struct table* table, size_t* size)
{
    size_t i;
    void* data;
    char* ptr;

    const size_t count = BUCKET_SIZE(table->indices) + BUCKET_DATA_INDEX;
    *size = BUCKET_SIZE(table->indices);

    data = malloc(*size * table->bytes);
    ptr = data;
    
    for (i = BUCKET_DATA_INDEX; i < count; ++i) {
        memcpy(ptr, _table_at(table, table->indices[i]), table->bytes);
        ptr += table->bytes;
    }

    return data;
}

void* table_values(const struct table* table)
{
    return table->data;
}

void* table_value_at(const struct table* table, const size_t index)
{
    return _table_value_at(table, index);
}

size_t* table_indices(const struct table* table)
{
    return table->indices + BUCKET_DATA_INDEX;
}

size_t table_index_at(const struct table* table, const size_t index)
{
    return _table_index_at(table, index);
}

size_t table_indices_size(const struct table* table)
{
    return BUCKET_SIZE(table->indices);
}

size_t table_values_size(const struct table* table)
{
    return table->size;
}

size_t table_bytes(const struct table* table)
{
    return table->bytes;
}

void table_free(struct table* table)
{
    if (table->data) {
        free(table->data);
        free(table->indices);

        table->data = NULL;
        table->indices = NULL;
        table->size = 0;
        table->capacity = 0;
    }
}

#endif /* UTOPIA_TABLE_IMPLEMENTATION */
#endif /* UTOPIA_IMPLEMENTATION */
