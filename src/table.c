#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/**********************************
 -> Indexed Table Data Structure <- 
**********************************/

static inline size_t memcap(const size_t memlen)
{
    size_t i;
    for (i = !!memlen; i && i < memlen; i <<= 1) {}
    return i;
}

static inline size_t idxcnt(const size_t* indices)
{
    size_t i;
    for (i = 0; indices[i]; ++i) {}
    return i;
}

static inline void table_push_index(table_t* restrict table, const size_t index)
{
    size_t cnt;

    if (!table->indices) {
        table->indices = malloc(sizeof(size_t) * 2);
        cnt = 0;
    } else {
        cnt = idxcnt(table->indices);
        const size_t len = (cnt + 1) * sizeof(size_t);
        const size_t cap = memcap(len);

        if (len >= cap) {
            table->indices = realloc(table->indices, cap * 2);
        }
    }

    table->indices[cnt] = index + 1;
    table->indices[cnt + 1] = 0;
}

static inline size_t table_push_data(table_t* restrict table, const void* restrict data)
{
    const size_t bytes = table->bytes, count = table->size;
    const char* ptr = table->data;
    for (size_t i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    
    const size_t len = bytes * count;
    const size_t cap = memcap(len);

    if (len >= cap) {
        table->data = !table->data ? malloc(memcap(bytes)) : realloc(table->data, cap * 2);
    }

    memcpy(_array_index(table, table->size++), data, bytes);

    return 0;
}

table_t table_create(const size_t bytes)
{
    table_t table;
    table.indices = NULL;
    table.data = NULL;
    table.bytes = bytes;
    table.size = 0;
    return table;
}

size_t table_push(table_t* restrict table, const void* restrict data)
{
    size_t search = table_push_data(table, data);
    search = !search ? table->size - 1: search - 1;
    table_push_index(table, search);
    return search;
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
    for (size_t i = 0; i < table->indices[i]; ++i) {
        array_push(&array, _array_index(table, table->indices[i] - 1));
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

size_t* table_indices(const table_t* restrict table)
{
    return table->indices;
}

size_t table_index_at(const table_t* restrict table, const size_t index)
{
    return _table_index_at(table, index);
}

size_t table_indices_size(const table_t* restrict table)
{
    return idxcnt(table->indices);
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
    }

    table->data = NULL;
    table->indices = NULL;
    table->size = 0;
}
