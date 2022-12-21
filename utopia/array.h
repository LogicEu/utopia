#ifndef UTOPIA_ARRAY_H
#define UTOPIA_ARRAY_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

struct array {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
};

#define _array_create(bytes) (struct array){NULL, bytes, 0, 0}
#define _array_index(array, i) ((char*)(array)->data + (i) * (array)->bytes)

struct array array_create(const size_t bytes);
struct array array_reserve(const size_t bytes, const size_t reserve);
struct array array_sized(const size_t bytes, const size_t size);
struct array array_wrap(void* data, const size_t bytes, const size_t size);
struct array array_copy(const struct array* array);
struct array array_move(struct array* array);
void array_push(struct array* array, const void* data);
void array_push_vargs(struct array* array, const size_t argc, ...);
void array_push_block(struct array* array, const void* data, const size_t count);
void array_push_block_at(struct array* array, const void* data, 
                        const size_t count, const size_t index);
void array_remove(struct array* array, const size_t index);
void array_remove_block(struct array* array, const size_t start, const size_t end);
void* array_data(const struct array* array);
void* array_peek(const struct array* array);
void* array_pop(struct array* array);
void* array_index(const struct array* array, const size_t index);
size_t array_bytes(const struct array* array);
size_t array_size(const struct array* array);
size_t array_capacity(const struct array* array);
size_t array_push_if(struct array* array, const void* data);
size_t array_search(const struct array* array, const void* data);
size_t* array_search_all(const struct array* array, const void* data);
void array_restructure(struct array* array, const size_t bytes);
void array_resize(struct array* array, const size_t size);
void array_cut(struct array* array);
void array_set(struct array* array);
void array_clear(struct array* array);
void array_free(struct array* array);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_ARRAY_H */
