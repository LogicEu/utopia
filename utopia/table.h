#ifndef UTOPIA_TABLE_H
#define UTOPIA_TABLE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>
#include <utopia/array.h>

typedef struct table {
    index_t* indices;
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
} table_t;

#define _table_index_at(table, i) ((table)->indices[i + 2])
#define _table_value_at(table, i) (_array_index((table), _table_index_at(table, i)))

table_t table_create(const size_t bytes);
size_t table_push(table_t* table, const void* data);
void table_push_index(table_t* table, const index_t index);
void table_push_data(table_t* table, const void* data);
void table_remove(table_t* table, const index_t index);
table_t table_compress(const array_t* buffer);
array_t table_decompress(const table_t* table);
void* table_values(const table_t* table);
void* table_value_at(const table_t* table, const size_t index);
index_t* table_indices(const table_t* table);
index_t table_index_at(const table_t* table, const size_t index);
size_t table_indices_size(const table_t* table);
size_t table_values_size(const table_t* table);
size_t table_bytes(const table_t* table);
void table_free(table_t* table);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_TABLE_H */
