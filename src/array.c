#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array_create(const size_t bytes)
{
    array_t array;
    array.data = NULL;
    array.bytes = bytes;
    array.capacity = 0;
    array.size = 0;
    return array;
}

array_t array_reserve(const size_t bytes, const size_t reserve)
{
    array_t array;
    array.data = malloc(reserve * bytes);
    array.bytes = bytes;
    array.capacity = reserve;
    array.size = 0;
    return array;
}

array_t array_copy(const array_t* restrict array)
{
    array_t ret;
    ret.data = malloc(array->capacity * array->bytes);
    ret.bytes = array->bytes;
    ret.capacity = array->capacity;
    ret.size = array->size;
    memcpy(ret.data, array->data, ret.size * ret.bytes);
    return ret;
}

void array_push(array_t* restrict array, const void* restrict data)
{
    if (array->size >= array->capacity) {
        array->capacity = array->capacity * !!array->capacity * 2 + !array->capacity;
        array->data = !array->data ? malloc(array->capacity * array->bytes) : realloc(array->data, array->capacity * array->bytes);
    }
    memcpy(_array_index(array, array->size++), data, array->bytes);
}

void array_remove(array_t* restrict array, const size_t index)
{
    char* ptr = _array_index(array, index);
    memmove(ptr, ptr + array->bytes, (--array->size - index) * array->bytes);
}

void* array_index(const array_t* restrict array, const size_t index)
{   
    return _array_index(array, index);
}

void* array_peek(const array_t* restrict array)
{
    return !array->size ? NULL : _array_index(array, array->size - 1);
}

void* array_pop(array_t* restrict array)
{
    return !array->size ? NULL : _array_index(array, --array->size);
}

void array_resize(array_t* restrict array, const size_t size)
{
    array->capacity = size * (size >= array->size) + array->size * (array->size > size) + !size;
    array->data = !array->data ? malloc(array->capacity * array->bytes) : realloc(array->data, array->capacity * array->bytes);
}

void array_cut(array_t* restrict array)
{
    if (!array->size) return;
    array->capacity = array->size;
    array->data = realloc(array->data, array->capacity * array->bytes);
}

size_t array_find(const array_t* restrict array, const void* restrict data)
{
    const size_t bytes = array->bytes, count = array->size;
    char* ptr = array->data;
    for (size_t i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    return 0;
}

size_t array_push_if(array_t* restrict array, const void* restrict data)
{
    const size_t bytes = array->bytes, count = array->size;
    char* ptr = array->data;
    for (size_t i = 0; i < count; ++i, ptr += bytes) {
        if (!memcmp(ptr, data, bytes)) {
            return i + 1;
        }
    }
    array_push(array, data);
    return 0;
}

void array_set(array_t* restrict array)
{
    const size_t bytes = array->bytes;
    const char* data = array->data;
    for (size_t i = 0; i < array->size; ++i) {
        for (size_t j = i + 1; j < array->size; ++j) {
            if (!memcmp(data + i * bytes, data + j * bytes, bytes)) {
                array_remove(array, j--);
            }
        }
    } 
}

void array_clear(array_t* restrict array)
{
    array->size = 0;
}

void array_free(array_t* restrict array)
{
    if (array->data) free(array->data);
    array->data = NULL;
    array->capacity = 0;
    array->size = 0;
}
