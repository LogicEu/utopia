#include <utopia/array.h>
#include USTDLIB_H
#include USTRING_H
#include USTDARG_H

/***************************
 -> Dynamic Generic Array <- 
***************************/

struct array array_create(const size_t bytes)
{
    struct array array;
    array.bytes = bytes + !bytes;
    array.data = NULL;
    array.capacity = 0;
    array.size = 0;
    return array;
}

struct array array_reserve(const size_t bytes, const size_t reserve)
{
    struct array array;
    array.bytes = bytes + !bytes;
    array.data = reserve ? malloc(reserve * array.bytes) : NULL; 
    array.capacity = reserve;
    array.size = 0;
    return array;
}

struct array array_sized(const size_t bytes, const size_t size)
{
    struct array array;
    array.bytes = bytes + !bytes;
    array.data = size ? calloc(size, array.bytes) : NULL; 
    array.capacity = size;
    array.size = size;
    return array;
}

struct array array_copy(const struct array* array)
{
    struct array ret;
    ret.data = malloc(array->capacity * array->bytes);
    ret.bytes = array->bytes;
    ret.capacity = array->capacity;
    ret.size = array->size;
    
    memcpy(ret.data, array->data, ret.size * ret.bytes);
    return ret;
}

struct array array_move(struct array* array)
{
    struct array ret;
    ret.data = array->data;
    ret.bytes = array->bytes;
    ret.capacity = array->capacity;
    ret.size = array->size;
    
    array->size = 0;
    array->capacity = 0;
    array->data = NULL;

    return ret;
}

struct array array_wrap(void* data, const size_t bytes, const size_t size)
{
    struct array array;
    array.bytes = bytes + !bytes;
    array.data = data;
    array.capacity = size + !size;
    array.size = array.capacity;
    return array;
}

void array_push(struct array* array, const void* data)
{
    if (array->size == array->capacity) {
        array->capacity = array->capacity * 2 + !array->capacity;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    memcpy(_array_index(array, array->size++), data, array->bytes);
}

void array_push_block(struct array* array, const void* data, const size_t count)
{
    while (array->size + count > array->capacity) {
        array->capacity += count + 1;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    memcpy(_array_index(array, array->size), data, count * array->bytes);
    array->size += count;
}

void array_push_block_at(struct array* array, const void* data, 
                        const size_t count, const size_t index)
{
    char* ptr;
    if (array->size + count >= array->capacity) {
        array->capacity += count + 1;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    
    ptr = _array_index(array, index);
    memcpy(ptr + count * array->bytes, ptr, (array->size - index) * array->bytes);
    memcpy(ptr, data, count * array->bytes);
    array->size += count;
}

void array_push_vargs(struct array* array, const size_t argc, ...)
{
    size_t i;
    va_list args;
    va_start(args, argc);
    for (i = 0; i < argc; ++i) {
        array_push(array, va_arg(args, void*));
    }
    va_end(args);
}

void array_remove(struct array* array, const size_t index)
{
    char* ptr = _array_index(array, index);
    memmove(ptr, ptr + array->bytes, (--array->size - index) * array->bytes);
}

void array_remove_block(struct array* array, const size_t start, const size_t end)
{
    char* p = _array_index(array, start);
    size_t size = end - start;
    memmove(p, p + size * array->bytes, (array->size - end) * array->bytes);
    array->size -= size;
}

void* array_data(const struct array* array)
{
    return array->data;
}

void* array_peek(const struct array* array)
{
    return !array->size ? NULL : _array_index(array, array->size - 1);
}

void* array_pop(struct array* array)
{
    return !array->size ? NULL : _array_index(array, --array->size);
}

void* array_index(const struct array* array, const size_t index)
{   
    return _array_index(array, index);
}

size_t array_bytes(const struct array* array)
{
    return array->bytes;
}

size_t array_size(const struct array* array)
{
    return array->size;
}

size_t array_capacity(const struct array* array)
{
    return array->capacity;
}

size_t array_search(const struct array* array, const void* data)
{
    size_t i;
    const size_t bytes = array->bytes, count = array->size;
    const char* ptr = array->data;
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    return 0;
}

size_t* array_search_all(const struct array* array, const void* data)
{
    size_t i, n;
    const size_t bytes = array->bytes, count = array->size;
    const char* ptr = array->data;
    
    struct array search = array_create(sizeof(size_t));
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            n = i + 1;
            array_push(&search, &n);
        }
    }

    n = 0;
    array_push(&search, &n);
    return search.data;
}

size_t array_push_if(struct array* array, const void* data)
{
    size_t i;
    const size_t bytes = array->bytes, count = array->size;
    const char* ptr = array->data;
    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    
    array_push(array, data);
    return 0;
}

void array_set(struct array* array)
{
    size_t i, j;
    const size_t bytes = array->bytes;
    const char* data = array->data;
    for (i = 0; i < array->size; ++i) {
        for (j = i + 1; j < array->size; ++j) {
            if (!memcmp(data + i * bytes, data + j * bytes, bytes)) {
                array_remove(array, j--);
            }
        }
    } 
}

void array_resize(struct array* array, const size_t size)
{
    array->capacity = (size > array->size) ? size : array->size;
    if (array->capacity) {
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
}

void array_cut(struct array* array)
{
    array->capacity = array->size + !array->size;
    array->data = realloc(array->data, array->capacity * array->bytes);
}

void array_restructure(struct array* array, const size_t bytes)
{
    const size_t n = bytes + !bytes;
    array->capacity = (array->capacity * array->bytes) / n;
    array->size = (array->size * array->bytes) / n;
    array->bytes = n;
}

void array_clear(struct array* array)
{
    array->size = 0;
}

void array_free(struct array* array)
{
    if (array->data) {
        free(array->data);
        array->data = NULL;
        array->capacity = 0;
        array->size = 0;
    }
}
