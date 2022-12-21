#include <utopia/table.h>
#include USTDLIB_H
#include USTRING_H
#include "bucket.h"

/**********************************
 -> Indexed Table Data Structure <- 
**********************************/

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

void table_push_index(struct table* table, const index_t index)
{
    table->indices = bucket_push(table->indices, index);
}

void table_push_data(struct table* table, const void* data)
{ 
    if (table->size == table->capacity) {
        table->capacity = table->capacity * 2 + !table->capacity;
        table->data = realloc(table->data, table->capacity * table->bytes);
    }
    memcpy(_vector_index(table, table->size++), data, table->bytes);
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

void table_remove(struct table* table, const index_t index)
{
    if (table->indices) {
        size_t size;
        index_t* indices, i;
        char* ptr = _vector_index(table, index);
        memmove(ptr, ptr + table->bytes, (--table->size - index) * table->bytes);
        
        indices = table_indices(table);
        size = table_indices_size(table);
        for (i = 0; i < size; ++i) {
            if (indices[i] == index) {
                bucket_remove(table->indices, (index_t)i);
            } 
            else if (table->indices[i] > index) {
                --table->indices[i];
            }
        }
    }
}

struct table table_compress(const struct vector* buffer)
{   
    size_t i;
    const size_t bsize = buffer->size;
    const size_t bytes = buffer->bytes;

    struct table table = table_create(bytes);

    char* ptr = buffer->data;
    for (i = 0; i < bsize; ++i, ptr += bytes) {
        table_push(&table, ptr);
    }

    return table;
}

struct vector table_decompress(const struct table* table)
{
    size_t i;
    struct vector vector = vector_create(table->bytes);
    const size_t size = bucket_size(table->indices) + BUCKET_DATA_INDEX;
    for (i = BUCKET_DATA_INDEX; i < size; ++i) {
        vector_push(&vector, _vector_index(table, table->indices[i]));
    }
    return vector;
}

void* table_values(const struct table* table)
{
    return table->data;
}

void* table_value_at(const struct table* table, const size_t index)
{
    return _table_value_at(table, index);
}

index_t* table_indices(const struct table* table)
{
    return table->indices + BUCKET_DATA_INDEX;
}

index_t table_index_at(const struct table* table, const size_t index)
{
    return _table_index_at(table, index);
}

size_t table_indices_size(const struct table* table)
{
    return bucket_size(table->indices);
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
