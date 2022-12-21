#ifndef UTOPIA_ARRAY_H
#define UTOPIA_ARRAY_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct vector {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
};

#define _vector_create(bytes) (struct vector){NULL, bytes, 0, 0}
#define _vector_index(vector, i) ((char*)(vector)->data + (i) * (vector)->bytes)

struct vector vector_create(const size_t bytes);
struct vector vector_reserve(const size_t bytes, const size_t reserve);
struct vector vector_sized(const size_t bytes, const size_t size);
struct vector vector_wrap(void* data, const size_t bytes, const size_t size);
struct vector vector_copy(const struct vector* vector);
struct vector vector_move(struct vector* vector);
void vector_push(struct vector* vector, const void* data);
void vector_push_vargs(struct vector* vector, const size_t argc, ...);
void vector_push_block(struct vector* vector, const void* data, const size_t count);
void vector_push_block_at(struct vector* vector, const void* data, 
                        const size_t count, const size_t index);
void vector_remove(struct vector* vector, const size_t index);
void vector_remove_block(struct vector* vector, const size_t start, const size_t end);
void* vector_data(const struct vector* vector);
void* vector_peek(const struct vector* vector);
void* vector_pop(struct vector* vector);
void* vector_index(const struct vector* vector, const size_t index);
size_t vector_bytes(const struct vector* vector);
size_t vector_size(const struct vector* vector);
size_t vector_capacity(const struct vector* vector);
size_t vector_push_if(struct vector* vector, const void* data);
size_t vector_search(const struct vector* vector, const void* data);
size_t* vector_search_all(const struct vector* vector, const void* data);
void vector_restructure(struct vector* vector, const size_t bytes);
void vector_resize(struct vector* vector, const size_t size);
void vector_cut(struct vector* vector);
void vector_set(struct vector* vector);
void vector_clear(struct vector* vector);
void vector_free(struct vector* vector);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_ARRAY_H */
