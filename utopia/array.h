#ifndef UTOPIA_ARRAY_H
#define UTOPIA_ARRAY_H

#include <utopia/types.h>

typedef struct array {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t size;
} array_t;

#define _array_create(bytes) (array_t){NULL, bytes, 0, 0}
#define _array_index(array, i) ((char*)(array)->data + (i) * (array)->bytes)

array_t array_create(const size_t bytes);
array_t array_reserve(const size_t bytes, const size_t reserve);
array_t array_sized(const size_t bytes, const size_t size);
array_t array_wrap(void* data, const size_t bytes, const size_t size);
array_t array_copy(const array_t* array);
array_t array_move(array_t* array);
void array_push(array_t* array, const void* data);
void array_push_vargs(array_t* array, const size_t argc, ...);
void array_push_block(array_t* array, const void* data, const size_t count);
void array_push_block_at(array_t* array, const void* data, 
                        const size_t count, const size_t index);
void array_remove(array_t* array, const size_t index);
void array_remove_block(array_t* array, const size_t start, const size_t end);
void* array_data(const array_t* array);
void* array_peek(const array_t* array);
void* array_pop(array_t* array);
void* array_index(const array_t* array, const size_t index);
size_t array_bytes(const array_t* array);
size_t array_size(const array_t* array);
size_t array_capacity(const array_t* array);
size_t array_push_if(array_t* array, const void* data);
size_t array_search(const array_t* array, const void* data);
size_t* array_search_all(const array_t* array, const void* data);
void array_restructure(array_t* array, const size_t bytes);
void array_resize(array_t* array, const size_t size);
void array_cut(array_t* array);
void array_set(array_t* array);
void array_clear(array_t* array);
void array_free(array_t* array);

#endif /* UTOPIA_ARRAY_H */
