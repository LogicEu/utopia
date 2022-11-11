#include <utopia.h>
#include USTDLIB_H
#include USTRING_H
#include USTDARG_H

/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array_create(const size_t bytes)
{
    array_t array;
    array.bytes = bytes + !bytes;
    array.data = NULL;
    array.capacity = 0;
    array.size = 0;
    return array;
}

array_t array_reserve(const size_t bytes, const size_t reserve)
{
    array_t array;
    array.bytes = bytes + !bytes;
    array.data = reserve ? malloc(reserve * array.bytes) : NULL; 
    array.capacity = reserve;
    array.size = 0;
    return array;
}

array_t array_sized(const size_t bytes, const size_t size)
{
    array_t array;
    array.bytes = bytes + !bytes;
    array.data = size ? calloc(size, array.bytes) : NULL; 
    array.capacity = size;
    array.size = size;
    return array;
}

array_t array_copy(const array_t*array)
{
    array_t ret;
    ret.data = malloc(array->capacity * array->bytes);
    ret.bytes = array->bytes;
    ret.capacity = array->capacity;
    ret.size = array->size;
    
    memcpy(ret.data, array->data, ret.size * ret.bytes);
    return ret;
}

array_t array_move(array_t*array)
{
    array_t ret;
    ret.data = array->data;
    ret.bytes = array->bytes;
    ret.capacity = array->capacity;
    ret.size = array->size;
    
    array->size = 0;
    array->capacity = 0;
    array->data = NULL;

    return ret;
}

array_t array_wrap(void*data, const size_t bytes, const size_t size)
{
    array_t array;
    array.bytes = bytes + !bytes;
    array.data = data;
    array.capacity = size + !size;
    array.size = array.capacity;
    return array;
}

void array_push(array_t*array, const void*data)
{
    if (array->size == array->capacity) {
        array->capacity = array->capacity * 2 + !array->capacity;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    memcpy(_array_index(array, array->size++), data, array->bytes);
}

void array_push_block(array_t* array, const void* data, const size_t count)
{
    while (array->size + count > array->capacity) {
        array->capacity += count + 1;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    memcpy(_array_index(array, array->size), data, count * array->bytes);
    array->size += count;
}

void array_push_block_at(array_t*array, const void*data, 
                        const size_t count, const size_t index)
{
    if (array->size + count >= array->capacity) {
        array->capacity += count + 1;
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
    char* ptr = _array_index(array, index);
    memcpy(ptr + count * array->bytes, ptr, (array->size - index) * array->bytes);
    memcpy(ptr, data, count * array->bytes);
    array->size += count;
}

void array_push_vargs(array_t* array, const size_t argc, ...)
{
    size_t i;
    va_list args;
    va_start(args, argc);
    for (i = 0; i < argc; ++i) {
        array_push(array, va_arg(args, void*));
    }
    va_end(args);
}

void array_remove(array_t*array, const size_t index)
{
    char* ptr = _array_index(array, index);
    memmove(ptr, ptr + array->bytes, (--array->size - index) * array->bytes);
}

void array_remove_block(array_t*array, const size_t start, const size_t end)
{
    char* p = _array_index(array, start);
    size_t size = end - start;
    memmove(p, p + size * array->bytes, (array->size - end) * array->bytes);
    array->size -= size;
}

void* array_data(const array_t*array)
{
    return array->data;
}

void* array_peek(const array_t*array)
{
    return !array->size ? NULL : _array_index(array, array->size - 1);
}

void* array_pop(array_t*array)
{
    return !array->size ? NULL : _array_index(array, --array->size);
}

void* array_index(const array_t*array, const size_t index)
{   
    return _array_index(array, index);
}

size_t array_bytes(const array_t*array)
{
    return array->bytes;
}

size_t array_size(const array_t*array)
{
    return array->size;
}

size_t array_capacity(const array_t*array)
{
    return array->capacity;
}

size_t array_search(const array_t*array, const void*data)
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

size_t* array_search_all(const array_t*array, const void*data)
{
    const size_t bytes = array->bytes, count = array->size;
    const char* ptr = array->data;

    size_t indices[count], i, j = 0;

    for (i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            indices[j++] = i + 1;
        }
    }

    if (!j) {
        return NULL;
    }

    const size_t elements = j;
    size_t* idx = malloc((elements + 1) * sizeof(size_t));
    memcpy(idx, indices, elements * sizeof(size_t));
    idx[elements] = 0;

    return idx;
}

size_t array_push_if(array_t*array, const void*data)
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

void array_set(array_t*array)
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

void array_resize(array_t*array, const size_t size)
{
    array->capacity = (size > array->size) ? size : array->size;
    if (array->capacity) {
        array->data = realloc(array->data, array->capacity * array->bytes);
    }
}

void array_cut(array_t*array)
{
    array->capacity = array->size + !array->size;
    array->data = realloc(array->data, array->capacity * array->bytes);
}

void array_restructure(array_t*array, const size_t bytes)
{
    const size_t n = bytes + !bytes;
    array->capacity = (array->capacity * array->bytes) / n;
    array->size = (array->size * array->bytes) / n;
    array->bytes = n;
}

void array_clear(array_t*array)
{
    array->size = 0;
}

void array_free(array_t*array)
{
    if (array->data) {
        free(array->data);
        array->data = NULL;
        array->capacity = 0;
        array->size = 0;
    }
}
