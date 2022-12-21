#ifndef UTOPIA_TABLE_H
#define UTOPIA_TABLE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>
#include <utopia/vector.h>

struct table {
    index_t* indices;
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
};

#define _table_index_at(table, i) ((table)->indices[i + 2])
#define _table_value_at(table, i) (_vector_index((table), _table_index_at(table, i)))

struct table table_create(const size_t bytes);
size_t table_push(struct table* table, const void* data);
void table_push_index(struct table* table, const index_t index);
void table_push_data(struct table* table, const void* data);
void table_remove(struct table* table, const index_t index);
struct table table_compress(const struct vector* buffer);
struct vector table_decompress(const struct table* table);
void* table_values(const struct table* table);
void* table_value_at(const struct table* table, const size_t index);
index_t* table_indices(const struct table* table);
index_t table_index_at(const struct table* table, const size_t index);
size_t table_indices_size(const struct table* table);
size_t table_values_size(const struct table* table);
size_t table_bytes(const struct table* table);
void table_free(struct table* table);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_TABLE_H */
