#include <utopia.h>
#include <stdlib.h>
#include <string.h>

string_t string_create(const char* restrict data)
{
    const size_t len = data ? strlen(data) : 0;
    
    string_t str;
    str.capacity = len + !!data;
    str.size = len;
    str.data = str.capacity ? malloc(str.capacity) : NULL;
    memcpy(str.data, data, str.capacity);

    return str;
}

string_t string_copy(const string_t* restrict str)
{
    string_t ret;
    ret.capacity = str->capacity;
    ret.size = str->size;
    ret.data = ret.capacity ? malloc(ret.capacity) : NULL;
    memcpy(ret.data, str->data, ret.size + 1);
    return ret;
}

void string_push(string_t* restrict str, const char* restrict buffer)
{
    const size_t len = strlen(buffer);
    if (str->size + len + 1 >= str->capacity) {
        str->capacity = str->capacity * 2 + len + 1;
        str->data = realloc(str->data, str->capacity);
    }
    memcpy(str->data + str->size, buffer, len + 1);
    str->size += len;
}

void string_concat(string_t* restrict str1, const string_t* restrict str2)
{
    if (str1->size + str2->size + 1 >= str1->capacity) {
        str1->capacity = str1->capacity * 2 + str2->size + 1;
        str1->data = realloc(str1->data, str1->capacity);
    }
    memcpy(str1->data + str1->size, str2->data, str2->size + 1);
    str1->size += str2->size;
}

size_t string_capacity(const string_t* restrict str)
{
    return str->capacity;
}

size_t string_size(const string_t* restrict str)
{
    return str->size;
}

char* string_data(const string_t* restrict str)
{
    return str->data;
}

size_t string_search(const string_t* restrict str, const char* restrict search)
{
    if (str->data) {
        char* find = strstr(str->data, search);
        if (find) {
            return find - str->data;
        }
    }
    return 0;
}

size_t* string_search_all(const string_t* restrict str, const char* restrict search)
{
    if (str->data) {
        char* find = str->data;
        size_t index[str->size], count = 0;
        while ((find = strstr(find, search))) {
            index[count++] = find - str->data;
            ++find;
        }

        if (count) {
            size_t* indices = malloc((count + 1) * sizeof(size_t));
            memcpy(indices, index, count * sizeof(size_t));
            indices[count] = 0;
            return indices;
        }
    }
    return NULL;
}

void string_remove(string_t* restrict str, const char* restrict search)
{
    if (str->data) {
        const size_t len = strlen(search);
        char* find = strstr(str->data, search);
        if (find) {
            memmove(find, find + len, (find - str->data) - len);
            str->size -= len;
        }
    }
}

void string_remove_all(string_t* restrict str, const char* restrict search)
{
    if (str->data) {
        const size_t len = strlen(search);
        char* find = str->data;
        while ((find = strstr(find, search))) {
            memmove(find, find + len, (find - str->data) - len);
            str->size -= len;
            ++find;
        }
    }
}

void string_reverse(string_t* restrict str)
{
    const size_t size = str->size;
    char buff[size + 1];
    for (size_t i = 0; i < size; ++i) {
        buff[size - i] = str->data[i];
    }
    memcpy(str->data, buff, size);
}

void string_clear(string_t* restrict str)
{
    str->size = 0;
}

void string_free(string_t* restrict str)
{
    if (str->data) {
        free(str->data);
        str->data = NULL;
        str->capacity = 0;
        str->size = 0;
    }
}