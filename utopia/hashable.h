#ifndef UTOPIA_HASHABLE_H
#define UTOPIA_HASHABLE_H

#include <utopia/types.h>
#include USTDLIB_H
#include USTRING_H

#ifndef UTOPIA_HASH_SIZE
#define UTOPIA_HASH_SIZE 32
#endif

static void* memdup(const void* src, size_t size)
{
    void* dup = malloc(size);
    memcpy(dup, src, size);
    return dup;
}

static size_t hash_cstr(const void* key)
{
    int c;
    size_t hash = 5381;
    const unsigned char* str = key;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_HASHABLE_H */
