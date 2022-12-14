#ifndef UTOPIA_STRING_H
#define UTOPIA_STRING_H

#include <utopia/types.h>

typedef struct string {
    char* data;
    size_t capacity;
    size_t size;
} string_t;

string_t string_create(const char* data);
string_t string_copy(const string_t* str);
string_t string_wrap(char* str);
string_t string_ranged(const char* from, const char* to);
string_t string_reserve(const size_t size);
string_t string_empty(void);
void string_push(string_t* str, const char* buffer);
void string_push_at(string_t* str, const char* buffer, const size_t size);
void string_concat(string_t* str1, const string_t* str2);
size_t string_capacity(const string_t* str);
size_t string_size(const string_t* str);
char* string_data(const string_t* str);
size_t string_search(const string_t* str, const char* search);
void string_remove(string_t* str, const char* search);
void string_remove_index(string_t* str, const size_t index);
void string_remove_range(string_t* str, const size_t from, const size_t to);
void string_remove_all(string_t* str, const char* search);
void string_reverse(string_t* str);
void string_clear(string_t* str);
void string_free(string_t* str);

#endif /* UTOPIA_STRING_H */
