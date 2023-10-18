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

#ifndef UTOPIA_VECTOR_TEMPLATE_H
#define UTOPIA_VECTOR_TEMPLATE_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/*************
Generic Vector
*************/

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDDEF_H
#include USTDLIB_H
#include USTRING_H

#define vector_decl(type)       \
struct type ## _vector {        \
    type* data;                 \
    size_t capacity;            \
    size_t size;                \
}

#define vector_create_decl(type) \
struct type ## _vector type ## _vector_create(void)
#define vector_reserve_decl(type) \
struct type ## _vector type ## _vector_reserve(const size_t)
#define vector_sized_decl(type) \
struct type ## _vector type ## _vector_sized(const size_t)
#define vector_wrap_decl(type) \
struct type ## _vector type ## _vector_wrap(type*, const size_t)
#define vector_copy_decl(type) \
struct type ## _vector type ## _vector_copy(const struct type ## _vector*)
#define vector_move_decl(type) \
struct type ## _vector type ## _vector_move(struct type ## _vector*)
#define vector_push_decl(type) \
type* type ## _vector_push(struct type ## _vector*, const type*)
#define vector_push_at_decl(type) \
type* type ## _vector_push_at(struct type ## _vector*, const type*, const size_t)
#define vector_push_block_decl(type) \
type* type ## _vector_push_block(struct type ## _vector*, const type*, const size_t)
#define vector_push_block_at_decl(type) \
type* type ## _vector_push_block_at(struct type ## _vector*, const type*, \
                                    const size_t, const size_t)
#define vector_remove_decl(type) \
void type ## _vector_remove(struct type ## _vector*, const size_t)
#define vector_peek_decl(type) \
type* type ## _vector_peek(const struct type ## _vector*)
#define vector_pop_decl(type) \
type* type ## _vector_pop(struct type ## _vector*)
#define vector_search_decl(type) \
size_t type ## _vector_search(const struct type ## _vector*, const type*)
#define vector_resize_decl(type) \
void type ## _vector_resize(struct type ## _vector*, const size_t)
#define vector_cut_decl(type) \
void type ## _vector_cut(struct type ## _vector* vector)
#define vector_clear_decl(type) \
void type ## _vector_clear(struct type ## _vector* vector)
#define vector_free_decl(type) \
void type ## _vector_free(struct type ## _vector* vector)

#define vector_template_decl(type) \
vector_decl(type);              \
vector_create_decl(type);       \
vector_reserve_decl(type);      \
vector_sized_decl(type);        \
vector_wrap_decl(type);         \
vector_copy_decl(type);         \
vector_move_decl(type);         \
vector_push_decl(type);         \
vector_push_at_decl(type);      \
vector_push_block_decl(type);   \
vector_push_block_at_decl(type);\
vector_remove_decl(type);       \
vector_peek_decl(type);         \
vector_pop_decl(type);          \
vector_search_decl(type);       \
vector_resize_decl(type);       \
vector_cut_decl(type);          \
vector_clear_decl(type);        \
vector_free_decl(type)

#define vector_template_impl(type) \
vector_create_impl(type)        \
vector_reserve_impl(type)       \
vector_sized_impl(type)         \
vector_wrap_impl(type)          \
vector_copy_impl(type)          \
vector_move_impl(type)          \
vector_push_impl(type)          \
vector_push_at_impl(type)       \
vector_push_block_impl(type)    \
vector_push_block_at_impl(type) \
vector_remove_impl(type)        \
vector_peek_impl(type)          \
vector_pop_impl(type)           \
vector_search_impl(type)        \
vector_resize_impl(type)        \
vector_cut_impl(type)           \
vector_clear_impl(type)         \
vector_free_impl(type)          \
typedef type utopia_ ## type

/*************
Generic Vector
*************/

#define vector_create_impl(type)                    \
struct type ## _vector type ## _vector_create(void) \
{                                                   \
    struct type ## _vector vector = {0};            \
    return vector;                                  \
}

#define vector_reserve_impl(type)                   \
struct type ## _vector type ## _vector_reserve(const size_t reserve)\
{                                                   \
    struct type ## _vector vector;                  \
    vector.data = reserve ? (type*)malloc(reserve * sizeof(type)) : NULL;\
    vector.capacity = reserve;                      \
    vector.size = 0;                                \
    return vector;                                  \
}

#define vector_sized_impl(type)                     \
struct type ## _vector type ## _vector_sized(const size_t size)\
{                                                   \
    struct type ## _vector vector;                  \
    vector.data = size ? (type*)calloc(size, sizeof(type)) : NULL;\
    vector.capacity = size;                         \
    vector.size = size;                             \
    return vector;                                  \
}

#define vector_wrap_impl(type)                      \
struct type ## _vector type ## _vector_wrap(type* data, const size_t size)\
{                                                   \
    struct type ## _vector vector;                  \
    vector.data = data;                             \
    vector.capacity = size;                         \
    vector.size = size;                             \
    return vector;                                  \
}

#define vector_copy_impl(type)                      \
struct type ## _vector type ## _vector_copy(const struct type ## _vector* vector)\
{                                                   \
    struct type ## _vector ret;                     \
    ret.data = (type*)malloc(vector->capacity * sizeof(type));\
    ret.capacity = vector->capacity;                \
    ret.size = vector->size;                        \
    memcpy(ret.data, vector->data, ret.size * sizeof(type));\
    return ret;                                     \
}

#define vector_move_impl(type)                      \
struct type ## _vector type ## _vector_move(struct type ## _vector* vector)\
{                                                   \
    struct type ## _vector ret = *vector;           \
    vector->size = 0;                               \
    vector->capacity = 0;                           \
    vector->data = NULL;                            \
    return ret;                                     \
}

#define vector_push_impl(type)                      \
type* type ## _vector_push(struct type ## _vector* vector, const type* data)\
{                                                   \
    if (vector->size == vector->capacity) {         \
        vector->capacity = (vector->capacity << 1) + !vector->capacity;\
        vector->data = (type*)realloc(vector->data, vector->capacity * sizeof(type));\
    }                                               \
                                                    \
    vector->data[vector->size] = *data;             \
    return vector->data + vector->size++;           \
}

#define vector_push_at_impl(type)                   \
type* type ## _vector_push_at(  struct type ## _vector* vector, const type* data,\
                                const size_t index) \
{                                                   \
    type* ptr;                                      \
    if (vector->size == vector->capacity) {         \
        vector->capacity = vector->capacity * 2 + !vector->capacity;\
        vector->data = realloc(vector->data, vector->capacity * sizeof(type));\
    }                                               \
    ptr = vector->data + index;                     \
    memcpy(ptr + 1, ptr, (vector->size++ - index) * sizeof(type));\
    *ptr = *data;                                   \
    return ptr;                                     \
}

#define vector_push_block_impl(type)                \
type* type ## _vector_push_block(struct type ## _vector* vector, const type* data,\
                                const size_t count) \
{                                                   \
    type* ptr;                                      \
    if (vector->size + count > vector->capacity) {  \
        vector->capacity += count + 1;              \
        vector->data = realloc(vector->data, vector->capacity * sizeof(type));\
    }                                               \
    ptr = vector->data + vector->size;              \
    memcpy(ptr, data, count * sizeof(type));        \
    vector->size += count;                          \
    return ptr;                                     \
}

#define vector_push_block_at_impl(type)             \
type* type ## _vector_push_block_at(struct type ## _vector* vector, const type* data,\
                                    const size_t count, const size_t index)\
{                                                   \
    type* ptr;\
    if (vector->size + count >= vector->capacity) { \
        vector->capacity += count + 1;              \
        vector->data = realloc(vector->data, vector->capacity * sizeof(type));\
    }                                               \
    ptr = vector->data + index;                     \
    memcpy(ptr + count, ptr, (vector->size - index) * sizeof(type));\
    memcpy(ptr, data, count * sizeof(type));        \
    vector->size += count;                          \
    return ptr;                                     \
}

#define vector_remove_impl(type)                    \
void type ## _vector_remove(struct type ## _vector* vector, const size_t index)\
{                                                   \
    type* ptr = vector->data + index;               \
    memmove(ptr, ptr + 1, (--vector->size - index) * sizeof(type));\
}

#define vector_peek_impl(type)                      \
type* type ## _vector_peek(const struct type ## _vector* vector)\
{                                                   \
    return !vector->size ? NULL : vector->data + vector->size - 1;\
}

#define vector_pop_impl(type)                       \
type* type ## _vector_pop(struct type ## _vector* vector)\
{                                                   \
    return !vector->size ? NULL : vector->data + --vector->size;\
}

#define vector_search_impl(type)                    \
size_t type ## _vector_search(const struct type ## _vector* vector, const type* data)\
{                                                   \
    size_t i;                                       \
    for (i = 0; i < vector->size; ++i) {            \
        if (!memcmp(vector->data + i, data, sizeof(type))) {\
            return i + 1;                           \
        }                                           \
    }                                               \
    return 0;                                       \
}

#define vector_resize_impl(type)                    \
void type ## _vector_resize(struct type ## _vector* vector, const size_t size)\
{                                                   \
    vector->capacity = (size > vector->size) ? size : vector->size;\
    if (vector->capacity) {                         \
        vector->data = (type*)realloc(vector->data, vector->capacity * sizeof(type));\
    }                                               \
}

#define vector_cut_impl(type)                       \
void type ## _vector_cut(struct type ## _vector* vector)\
{                                                   \
    vector->capacity = vector->size + !vector->size;\
    vector->data = (type*)realloc(vector->data, vector->capacity * sizeof(type));\
}

#define vector_clear_impl(type)                     \
void type ## _vector_clear(struct type ## _vector* vector)\
{                                                   \
    vector->size = 0;                               \
}

#define vector_free_impl(type)                      \
void type ## _vector_free(struct type ## _vector* vector)\
{                                                   \
    if (vector->data) {                             \
        free(vector->data);                         \
        vector->data = NULL;                        \
        vector->capacity = 0;                       \
        vector->size = 0;                           \
    }                                               \
}

#endif /* UTOPIA_VECTOR_TEMPLATE_H */

