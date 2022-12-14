#include <utopia/string.h>
#include USTDLIB_H
#include USTRING_H

string_t string_create(const char* data)
{
    const size_t len = data ? strlen(data) : 0;
    
    string_t str;
    str.capacity = len + !!len;
    str.size = len;
    str.data = str.capacity ? malloc(str.capacity) : NULL;
    memcpy(str.data, data, str.capacity);

    return str;
}

string_t string_copy(const string_t* str)
{
    string_t ret;
    ret.capacity = str->capacity;
    ret.size = str->size;
    ret.data = ret.capacity ? malloc(ret.capacity) : NULL;
    memcpy(ret.data, str->data, ret.size + 1);
    return ret;
}

string_t string_wrap(char* str)
{
    string_t ret;
    ret.size = str ? strlen(str) : 0;
    ret.capacity = ret.size + !!str;
    ret.data = str;
    return ret;
}

string_t string_ranged(const char* from, const char* to)
{
    string_t str;
    str.capacity = to - from + 1;
    str.size = str.capacity - 1;
    str.data = calloc(str.capacity, sizeof(char));
    memcpy(str.data, from, str.size);
    return str;
}

string_t string_reserve(const size_t size)
{
    string_t str;
    str.capacity = size;
    str.size = 0;
    str.data = str.capacity ? calloc(str.capacity, sizeof(char)) : NULL;
    return str;
}

string_t string_empty(void)
{
    string_t str;
    str.capacity = 0;
    str.size = 0;
    str.data = NULL;
    return str;
}

void string_push(string_t* str, const char* buffer)
{
    if (buffer) {
        const size_t len = strlen(buffer);
        if (str->size + len + 1 >= str->capacity) {
            str->capacity = str->capacity * 2 + len + 1;
            str->data = realloc(str->data, str->capacity);
        }
        memcpy(str->data + str->size, buffer, len + 1);
        str->size += len;
    }
}

void string_push_at(string_t* str, const char* buf, const size_t index)
{
    if (buf) {
        const size_t len = strlen(buf);
        if (str->size + len + 1 >= str->capacity) {
            str->capacity = str->capacity * 2 + len + 1;
            str->data = realloc(str->data, str->capacity);
        }
        memmove(str->data + index + len, str->data + index, str->size - index + 1);
        memcpy(str->data + index, buf, len);
        str->size += len;
    }
}

void string_concat(string_t* str1, const string_t* str2)
{
    if (str2->data) {
        if (str1->size + str2->size + 1 >= str1->capacity) {
            str1->capacity = str1->capacity * 2 + str2->size + 1;
            str1->data = realloc(str1->data, str1->capacity);
        }
        memcpy(str1->data + str1->size, str2->data, str2->size + 1);
        str1->size += str2->size;
    }
}

size_t string_capacity(const string_t* str)
{
    return str->capacity;
}

size_t string_size(const string_t* str)
{
    return str->size;
}

char* string_data(const string_t* str)
{
    return str->data;
}

size_t string_search(const string_t* str, const char* search)
{
    if (str->data) {
        char* find = strstr(str->data, search);
        if (find) {
            return find - str->data + 1;
        }
    }
    return 0;
}

void string_remove_index(string_t* str, const size_t index)
{
    if (str->data) {
        memmove(str->data + index, str->data + index + 1, str->size - index);
        str->data[--str->size] = 0;
    }
}

void string_remove_range(string_t* str, const size_t from, const size_t to)
{
    if (str->data) {
        memmove(str->data + from, str->data + to, str->size - to + 1);
        str->size -= to - from;
        str->data[str->size] = 0;
    }
}

void string_remove(string_t* str, const char* search)
{
    if (str->data) {
        const size_t len = strlen(search);
        char* find = strstr(str->data, search);
        if (find) {
            memmove(find, find + len, (find - str->data) - len);
            str->size -= len;
            str->data[str->size] = 0;
        }
    }
}

void string_remove_all(string_t* str, const char* search)
{
    if (str->data) {
        const size_t len = strlen(search);
        char* find = str->data;
        while ((find = strstr(find, search))) {
            memmove(find, find + len, (find - str->data) - len);
            str->size -= len;
            str->data[str->size] = 0;
            ++find;
        }
    }
}

void string_reverse(string_t* str)
{
    char c;
    size_t i, j;
    for (i = 0, j = str->size - 1; i < j; ++i, --j) {
        c = str->data[i];
        str->data[i] = str->data[j];
        str->data[j] = c;
    }
}

void string_clear(string_t* str)
{
    if (str->data) {
        str->data[0] = 0;
        str->size = 0;
    }
}

void string_free(string_t* str)
{
    if (str->data) {
        free(str->data);
        str->data = NULL;
        str->capacity = 0;
        str->size = 0;
    }
}
