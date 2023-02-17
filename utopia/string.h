/*  Copyright (c) 2022 Eugenio Arteaga A.

Permission is hereby granted, free of charge, to any 
person obtaining a copy of this software and associated 
documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice 
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#ifndef UTOPIA_STRING_H
#define UTOPIA_STRING_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/***********************
Dynamic String Container
************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct string {
    char* data;
    size_t capacity;
    size_t size;
};

struct string string_create(const char* data);
struct string string_copy(const struct string* str);
struct string string_wrap(char* str);
struct string string_ranged(const char* from, const char* to);
struct string string_reserve(const size_t size);
struct string string_empty(void);
char* string_push(struct string* str, const char* buffer);
char* string_push_at(struct string* str, const char* buffer, const size_t size);
char* string_concat(struct string* str1, const struct string* str2);
size_t string_capacity(const struct string* str);
size_t string_size(const struct string* str);
char* string_data(const struct string* str);
size_t string_search(const struct string* str, const char* search);
void string_remove(struct string* str, const char* search);
void string_remove_index(struct string* str, const size_t index);
void string_remove_range(struct string* str, const size_t from, const size_t to);
void string_remove_all(struct string* str, const char* search);
void string_reverse(struct string* str);
void string_clear(struct string* str);
void string_free(struct string* str);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_STRING_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_STRING_IMPLEMENTED
#define UTOPIA_STRING_IMPLEMENTED

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

/***********************
Dynamic String Container
************************/

struct string string_create(const char* data)
{
    const size_t len = data ? strlen(data) : 0;
    
    struct string str;
    str.capacity = len + !!len;
    str.size = len;
    str.data = str.capacity ? malloc(str.capacity) : NULL;
    memcpy(str.data, data, str.capacity);

    return str;
}

struct string string_copy(const struct string* str)
{
    struct string ret;
    ret.capacity = str->capacity;
    ret.size = str->size;
    ret.data = ret.capacity ? malloc(ret.capacity) : NULL;
    memcpy(ret.data, str->data, ret.size + 1);
    return ret;
}

struct string string_wrap(char* str)
{
    struct string ret;
    ret.size = str ? strlen(str) : 0;
    ret.capacity = ret.size + !!str;
    ret.data = str;
    return ret;
}

struct string string_ranged(const char* from, const char* to)
{
    struct string str;
    str.capacity = to - from + 1;
    str.size = str.capacity - 1;
    str.data = calloc(str.capacity, sizeof(char));
    memcpy(str.data, from, str.size);
    return str;
}

struct string string_reserve(const size_t size)
{
    struct string str;
    str.capacity = size;
    str.size = 0;
    str.data = str.capacity ? calloc(str.capacity, sizeof(char)) : NULL;
    return str;
}

struct string string_empty(void)
{
    struct string str;
    str.capacity = 0;
    str.size = 0;
    str.data = NULL;
    return str;
}

char* string_push(struct string* str, const char* buffer)
{
    char* ptr = NULL;
    if (buffer) {
        const size_t len = strlen(buffer);
        if (str->size + len + 1 >= str->capacity) {
            str->capacity = str->capacity * 2 + len + 1;
            str->data = realloc(str->data, str->capacity);
        }

        ptr = str->data + str->size;
        memcpy(ptr, buffer, len + 1);
        str->size += len;
    }
    return ptr;
}

char* string_push_at(struct string* str, const char* buf, const size_t index)
{
    char* ptr = NULL;
    if (buf) {
        const size_t len = strlen(buf);
        if (str->size + len + 1 >= str->capacity) {
            str->capacity = str->capacity * 2 + len + 1;
            str->data = realloc(str->data, str->capacity);
        }
        
        ptr = str->data + index;
        memmove(ptr + len, str->data + index, str->size - index + 1);
        memcpy(ptr, buf, len);
        str->size += len;
    }
    return ptr;
}

char* string_concat(struct string* str1, const struct string* str2)
{
    char* ptr = NULL;
    if (str2->data) {
        if (str1->size + str2->size + 1 >= str1->capacity) {
            str1->capacity = str1->capacity * 2 + str2->size + 1;
            str1->data = realloc(str1->data, str1->capacity);
        }
        
        ptr = str1->data + str1->size;
        memcpy(ptr, str2->data, str2->size + 1);
        str1->size += str2->size;
    }
    return ptr;
}

size_t string_capacity(const struct string* str)
{
    return str->capacity;
}

size_t string_size(const struct string* str)
{
    return str->size;
}

char* string_data(const struct string* str)
{
    return str->data;
}

size_t string_search(const struct string* str, const char* search)
{
    if (str->data) {
        char* find = strstr(str->data, search);
        if (find) {
            return find - str->data + 1;
        }
    }
    return 0;
}

void string_remove_index(struct string* str, const size_t index)
{
    if (str->data) {
        memmove(str->data + index, str->data + index + 1, str->size - index);
        str->data[--str->size] = 0;
    }
}

void string_remove_range(struct string* str, const size_t from, const size_t to)
{
    if (str->data) {
        memmove(str->data + from, str->data + to, str->size - to + 1);
        str->size -= to - from;
        str->data[str->size] = 0;
    }
}

void string_remove(struct string* str, const char* search)
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

void string_remove_all(struct string* str, const char* search)
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

void string_reverse(struct string* str)
{
    char c;
    size_t i, j;
    for (i = 0, j = str->size - 1; i < j; ++i, --j) {
        c = str->data[i];
        str->data[i] = str->data[j];
        str->data[j] = c;
    }
}

void string_clear(struct string* str)
{
    if (str->data) {
        str->data[0] = 0;
        str->size = 0;
    }
}

void string_free(struct string* str)
{
    if (str->data) {
        free(str->data);
        str->data = NULL;
        str->capacity = 0;
        str->size = 0;
    }
}

#endif /* UTOPIA_STRING_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
