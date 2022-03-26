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

array_t* array_copy(const array_t* restrict array)
{
    array_t* ret = malloc(sizeof(array_t));
    ret->bytes = array->bytes;
    ret->size = array->size;
    ret->used = array->used;
    ret->data = calloc(ret->size, ret->bytes);
    memcpy(ret->data, array->data, ret->used * ret->bytes);
    return ret;
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
    const size_t bytes = array->bytes;
    char* ptr = (char*)array->data + index * bytes;
    for (const char* end = (char*)array->data + (--array->used) * bytes; ptr != end; ptr += bytes) {
        memcpy(ptr, ptr + bytes, bytes);
    }
}

size_t array_find(const array_t* restrict array, const void* data)
{
    const size_t bytes = array->bytes;
    size_t index = 0;
    char* ptr = array->data;
    for (const char* end = ptr + array->used * bytes; ptr != end; ptr += bytes, ++index) {
        if (!memcmp(ptr, data, bytes)) return index + 1;
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
    const char* data = array->data;
    for (size_t i = 0; i < array->used; ++i) {
        for (size_t j = i + 1; j < array->used; ++j) {
            if (!memcmp(data + i * bytes, data + j * bytes, bytes)) {
                array_remove(array, j--);
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

void array_empy(array_t* array)
{
    if (!array->data) return;
    memset(array->data, 0, array->size * array->bytes);
    array->used = 0;
}

void array_free(array_t* array)
{
    if (!array->data) return;
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
