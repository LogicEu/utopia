#ifndef UTOPIA_VECTOR_H
#define UTOPIA_VECTOR_H

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

/*********************
Dynamic Generic Vector
*********************/

#ifdef UTOPIA_VECTOR_IMPL

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#ifndef USTDARG_H 
#define USTDARG_H <stdarg.h>
#endif

#include USTDLIB_H
#include USTRING_H
#include USTDARG_H

struct vector vector_create(const size_t bytes)
{
    struct vector vector;
    vector.bytes = bytes + !bytes;
    vector.data = NULL;
    vector.capacity = 0;
    vector.size = 0;
    return vector;
}

struct vector vector_reserve(const size_t bytes, const size_t reserve)
{
    struct vector vector;
    vector.bytes = bytes + !bytes;
    vector.data = reserve ? malloc(reserve * vector.bytes) : NULL; 
    vector.capacity = reserve;
    vector.size = 0;
    return vector;
}

struct vector vector_sized(const size_t bytes, const size_t size)
{
    struct vector vector;
    vector.bytes = bytes + !bytes;
    vector.data = size ? calloc(size, vector.bytes) : NULL; 
    vector.capacity = size;
    vector.size = size;
    return vector;
}

struct vector vector_copy(const struct vector* vector)
{
    struct vector ret;
    ret.data = malloc(vector->capacity * vector->bytes);
    ret.bytes = vector->bytes;
    ret.capacity = vector->capacity;
    ret.size = vector->size;
    
    memcpy(ret.data, vector->data, ret.size * ret.bytes);
    return ret;
}

struct vector vector_move(struct vector* vector)
{
    struct vector ret;
    ret.data = vector->data;
    ret.bytes = vector->bytes;
    ret.capacity = vector->capacity;
    ret.size = vector->size;
    
    vector->size = 0;
    vector->capacity = 0;
    vector->data = NULL;

    return ret;
}

struct vector vector_wrap(void* data, const size_t bytes, const size_t size)
{
    struct vector vector;
    vector.bytes = bytes + !bytes;
    vector.data = data;
    vector.capacity = size + !size;
    vector.size = vector.capacity;
    return vector;
}

void vector_push(struct vector* vector, const void* data)
{
    if (vector->size == vector->capacity) {
        vector->capacity = vector->capacity * 2 + !vector->capacity;
        vector->data = realloc(vector->data, vector->capacity * vector->bytes);
    }
    memcpy(_vector_index(vector, vector->size++), data, vector->bytes);
}

void vector_push_block(struct vector* vector, const void* data, const size_t count)
{
    while (vector->size + count > vector->capacity) {
        vector->capacity += count + 1;
        vector->data = realloc(vector->data, vector->capacity * vector->bytes);
    }
    memcpy(_vector_index(vector, vector->size), data, count * vector->bytes);
    vector->size += count;
}

void vector_push_block_at(struct vector* vector, const void* data, 
                        const size_t count, const size_t index)
{
    char* ptr;
    if (vector->size + count >= vector->capacity) {
        vector->capacity += count + 1;
        vector->data = realloc(vector->data, vector->capacity * vector->bytes);
    }
    
    ptr = _vector_index(vector, index);
    memcpy(ptr + count * vector->bytes, ptr, (vector->size - index) * vector->bytes);
    memcpy(ptr, data, count * vector->bytes);
    vector->size += count;
}

void vector_push_vargs(struct vector* vector, const size_t argc, ...)
{
    size_t i;
    va_list args;
    va_start(args, argc);
    for (i = 0; i < argc; ++i) {
        vector_push(vector, va_arg(args, void*));
    }
    va_end(args);
}

void vector_remove(struct vector* vector, const size_t index)
{
    char* ptr = _vector_index(vector, index);
    memmove(ptr, ptr + vector->bytes, (--vector->size - index) * vector->bytes);
}

void vector_remove_block(struct vector* vector, const size_t start, const size_t end)
{
    char* p = _vector_index(vector, start);
    size_t size = end - start;
    memmove(p, p + size * vector->bytes, (vector->size - end) * vector->bytes);
    vector->size -= size;
}

void* vector_data(const struct vector* vector)
{
    return vector->data;
}

void* vector_peek(const struct vector* vector)
{
    return !vector->size ? NULL : _vector_index(vector, vector->size - 1);
}

void* vector_pop(struct vector* vector)
{
    return !vector->size ? NULL : _vector_index(vector, --vector->size);
}

void* vector_index(const struct vector* vector, const size_t index)
{   
    return _vector_index(vector, index);
}

size_t vector_bytes(const struct vector* vector)
{
    return vector->bytes;
}

size_t vector_size(const struct vector* vector)
{
    return vector->size;
}

size_t vector_capacity(const struct vector* vector)
{
    return vector->capacity;
}

size_t vector_search(const struct vector* vector, const void* data)
{
    size_t i;
    const size_t bytes = vector->bytes, count = vector->size;
    const char* ptr = vector->data;
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    return 0;
}

size_t* vector_search_all(const struct vector* vector, const void* data)
{
    size_t i, n;
    const size_t bytes = vector->bytes, count = vector->size;
    const char* ptr = vector->data;
    
    struct vector search = vector_create(sizeof(size_t));
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            n = i + 1;
            vector_push(&search, &n);
        }
    }

    n = 0;
    vector_push(&search, &n);
    return search.data;
}

size_t vector_push_if(struct vector* vector, const void* data)
{
    size_t i;
    const size_t bytes = vector->bytes, count = vector->size;
    const char* ptr = vector->data;
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    
    vector_push(vector, data);
    return 0;
}

void vector_set(struct vector* vector)
{
    size_t i, j;
    const size_t bytes = vector->bytes;
    const char* data = vector->data;
    for (i = 0; i < vector->size; ++i) {
        for (j = i + 1; j < vector->size; ++j) {
            if (!memcmp(data + i * bytes, data + j * bytes, bytes)) {
                vector_remove(vector, j--);
            }
        }
    } 
}

void vector_resize(struct vector* vector, const size_t size)
{
    vector->capacity = (size > vector->size) ? size : vector->size;
    if (vector->capacity) {
        vector->data = realloc(vector->data, vector->capacity * vector->bytes);
    }
}

void vector_cut(struct vector* vector)
{
    vector->capacity = vector->size + !vector->size;
    vector->data = realloc(vector->data, vector->capacity * vector->bytes);
}

void vector_restructure(struct vector* vector, const size_t bytes)
{
    const size_t n = bytes + !bytes;
    vector->capacity = (vector->capacity * vector->bytes) / n;
    vector->size = (vector->size * vector->bytes) / n;
    vector->bytes = n;
}

void vector_clear(struct vector* vector)
{
    vector->size = 0;
}

void vector_free(struct vector* vector)
{
    if (vector->data) {
        free(vector->data);
        vector->data = NULL;
        vector->capacity = 0;
        vector->size = 0;
    }
}

#endif /* UTOPIA_VECTOR_IMPL */

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_VECTOR_H */
