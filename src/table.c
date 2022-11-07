#include <utopia.h>
#include USTDLIB_H
#include USTRING_H

#include "bucket.h"

/**********************************
 -> Indexed Table Data Structure <- 
**********************************/

table_t table_create(const size_t bytes)
{
    table_t table;
    table.indices = NULL;
    table.data = NULL;
    table.bytes = bytes + !bytes;
    table.size = 0;
    table.capacity = 0;
    return table;
}

size_t table_search(const table_t* restrict table, const void* restrict data)
{
    const size_t bytes = table->bytes, count = table->size;
    const char* ptr = table->data;
    for (size_t i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    return 0;
}

void table_push_index(table_t* restrict table, const index_t index)
{
    table->indices = bucket_push(table->indices, index);
}

void table_push_data(table_t* restrict table, const void* restrict data)
{ 
    if (table->size == table->capacity) {
        table->capacity = table->capacity * 2 + !table->capacity;
        table->data = realloc(table->data, table->capacity * table->bytes);
    }
    memcpy(_array_index(table, table->size++), data, table->bytes);
}

size_t table_push(table_t* restrict table, const void* restrict data)
{
    size_t search = table_search(table, data); 
    if (!search) {
        table_push_data(table, data);
        search = table->size;
    } 

    table_push_index(table, search - 1);
    return search;
}

void table_remove(table_t* restrict table, const index_t index)
{
    if (table->indices) {
        char* ptr = _array_index(table, index);
        memmove(ptr, ptr + table->bytes, (--table->size - index) * table->bytes);
        
        index_t* indices = table_indices(table);
        const size_t size = table_indices_size(table);
        for (size_t i = 0; i < size; ++i) {
            if (indices[i] == index) {
                bucket_remove(table->indices, (index_t)i);
            } 
            else if (table->indices[i] > index) {
                --table->indices[i];
            }
        }
    }
}

table_t table_compress(const array_t* restrict buffer)
{   
    const size_t bsize = buffer->size;
    const size_t bytes = buffer->bytes;

    table_t table = table_create(bytes);

    char* ptr = buffer->data;
    for (size_t i = 0; i < bsize; ++i, ptr += bytes) {
        table_push(&table, ptr);
    }

    return table;
}

array_t table_decompress(const table_t* restrict table)
{
    array_t array = array_create(table->bytes);
    const size_t size = bucket_size(table->indices) + BUCKET_DATA_INDEX;
    for (size_t i = BUCKET_DATA_INDEX; i < size; ++i) {
        array_push(&array, _array_index(table, table->indices[i]));
    }
    return array;
}

void* table_values(const table_t* restrict table)
{
    return table->data;
}

void* table_value_at(const table_t* restrict table, const size_t index)
{
    return _table_value_at(table, index);
}

index_t* table_indices(const table_t* restrict table)
{
    return table->indices + BUCKET_DATA_INDEX;
}

index_t table_index_at(const table_t* restrict table, const size_t index)
{
    return _table_index_at(table, index);
}

size_t table_indices_size(const table_t* restrict table)
{
    return bucket_size(table->indices);
}

size_t table_values_size(const table_t* restrict table)
{
    return table->size;
}

size_t table_bytes(const table_t* restrict table)
{
    return table->bytes;
}

void table_free(table_t* restrict table)
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
