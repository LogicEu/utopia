#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/***************************
 -> Dynamic Generic Array <- 
***************************/

array_t array_create(const size_t size, const size_t bytes)
{
    array_t array;
    array.data = calloc(size, bytes);
    array.bytes = bytes;
    array.size = size;
    array.used = 0;
    return array;
}

array_t* array_new(const size_t size, const size_t bytes)
{
    array_t* array = malloc(sizeof(array_t));
    array->data = calloc(size, bytes);
    array->bytes = bytes;
    array->size = size;
    array->used = 0;
    return array;
}

size_t array_is_empty(const array_t* restrict array)
{
    return array->used == 0;
}

size_t array_is_full(const array_t* restrict array)
{
    return array->used >= array->size;
}

void array_resize(array_t* array, const size_t size)
{
    if (size <= array->used) return;
    array->size = size;
    array->data = realloc(array->data, array->size * array->bytes);
}

void array_cut(array_t* array)
{
    array->size = array->used;
    array->data = realloc(array->data, array->size * array->bytes);
}

void* array_index(const array_t* restrict array, const size_t index)
{   
    return _array_index(array, index);
}

void array_push(array_t* array, const void* data)
{
    if (!array->data) array->data = calloc(array->size, array->bytes);
    if (array->used >= array->size) array_resize(array, array->size * 2);
    memcpy(_array_index(array, array->used++), data, array->bytes);
}

void array_remove(array_t* array, const size_t index)
{
    if (index >= array->used) return;
    char* ptr = (char*)array->data + index * array->bytes;
    for (const char* end = (char*)array->data + (--array->used) * array->bytes; ptr != end; ptr += array->bytes) {
        memcpy(ptr, ptr + array->bytes, array->bytes);
    }
}

size_t array_find(const array_t* restrict array, const void* data)
{
    size_t index = 0;
    char* ptr = array->data;
    for (const char* end = ptr + array->used * array->bytes; ptr != end; ptr += array->bytes, ++index) {
        if (!memcmp(ptr, data, array->bytes)) return index + 1;
    }
    return 0;
}

size_t array_push_if(array_t* array, const void* data)
{
    const size_t find = array_find(array, data);
    if (!find) array_push(array, data);
    return find;
}

void array_set(array_t* array)
{
    const size_t bytes = array->bytes;
    char* data = array->data;
    for (size_t i = 0; i < array->used; i += bytes) {
        for (size_t j = i + 1; j < array->used; j += bytes) {
            if (!memcmp(data + i, data + j, bytes)) {
                array_remove(array, j);
            }
        }
    } 
}

void* array_pop(array_t* array)
{
    if (!array->used) return NULL;
    return _array_index(array, --array->used);
}

void* array_peek(const array_t* restrict array)
{
    if (!array->used) return NULL;
    return _array_index(array, array->used - 1);
}

void array_free(array_t* array)
{
    if (!array || !array->data) return;
    free(array->data);
    array->data = NULL;
    array->used = 0;
}

void array_destroy(array_t* array)
{
    if (!array) return;
    array_free(array);
    free(array);
}
