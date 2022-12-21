#ifndef UTOPIA_STRING_H
#define UTOPIA_STRING_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

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
void string_push(struct string* str, const char* buffer);
void string_push_at(struct string* str, const char* buffer, const size_t size);
void string_concat(struct string* str1, const struct string* str2);
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

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_STRING_H */
