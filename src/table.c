#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/**********************************
 -> Indexed Table Data Structure <- 
**********************************/

table_t table_create(const size_t bytes)
{
    table_t table;
    table.values = array_create(bytes);
    table.indices = array_create(sizeof(size_t));
    return table;
}

size_t table_push(table_t* restrict table, const void* restrict data)
{
    size_t search = array_push_if(&table->values, data);
    search = !search ? table->values.size - 1 : search - 1;
    array_push(&table->indices, &search);
    return search;
}

table_t table_compress(const array_t* buffer)
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

array_t table_decompress(const table_t* table)
{
    array_t array = array_create(table->values.bytes);
    const size_t isize = table->indices.size;
    
    size_t* indices = table->indices.data;
    for (size_t i = 0; i < isize; ++i) {
        array_push(&array, array_index(&table->values, indices[i]));
    }

    return array;
}

void table_free(table_t* table)
{
    array_free(&table->values);
    array_free(&table->indices);
}
