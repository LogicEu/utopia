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
    array.size = 0;
    array.used = 0;
    return array;
}

array_t array_reserve(const size_t bytes, const size_t reserve)
{
    array_t array;
    array.data = malloc(reserve * bytes);
    array.bytes = bytes;
    array.size = reserve;
    array.used = 0;
    return array;
}

array_t array_copy(const array_t* restrict array)
{
    array_t ret;
    ret.data = malloc(array->size * array->bytes);
    ret.bytes = array->bytes;
    ret.size = array->size;
    ret.used = array->used;
    memcpy(ret.data, array->data, ret.used * ret.bytes);
    return ret;
}

void array_push(array_t* restrict array, const void* restrict data)
{
    if (array->used >= array->size) {
        array->size = array->size * !!array->size * 2 + !array->size;
        array->data = !array->data ? malloc(array->size * array->bytes) : realloc(array->data, array->size * array->bytes);
    }
    memcpy(_array_index(array, array->used++), data, array->bytes);
}

void array_remove(array_t* restrict array, const size_t index)
{
    char* ptr = _array_index(array, index);
    memmove(ptr, ptr + array->bytes, (--array->used - index) * array->bytes);
}

void array_resize(array_t* restrict array, const size_t size)
{
    array->size = size * (size >= array->used) + array->used * (array->used > size) + !size;
    array->data = !array->data ? malloc(array->size * array->bytes) : realloc(array->data, array->size * array->bytes);
}

void array_cut(array_t* restrict array)
{
    if (!array->used) return;
    array->size = array->used;
    array->data = realloc(array->data, array->size * array->bytes);
}

void* array_index(const array_t* restrict array, const size_t index)
{   
    return _array_index(array, index);
}

void* array_peek(const array_t* restrict array)
{
    return !array->used ? NULL : _array_index(array, array->used - 1);
}

void* array_pop(array_t* restrict array)
{
    return !array->used ? NULL : _array_index(array, --array->used);
}

size_t array_find(const array_t* restrict array, const void* restrict data)
{
    const size_t bytes = array->bytes, count = array->used;
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
    const size_t bytes = array->bytes, count = array->used;
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
    for (size_t i = 0; i < array->used; ++i) {
        for (size_t j = i + 1; j < array->used; ++j) {
            if (!memcmp(data + i * bytes, data + j * bytes, bytes)) {
                array_remove(array, j--);
            }
        }
    } 
}

void array_clear(array_t* restrict array)
{
    array->used = 0;
}

void array_free(array_t* restrict array)
{
    if (array->data) free(array->data);
    array->data = NULL;
    array->size = 0;
    array->used = 0;
}
