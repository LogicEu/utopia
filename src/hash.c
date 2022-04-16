#include <utopia.h>

/*******************************
 -> Hash Functions Prototypes <- 
*******************************/

size_t hash_cstr(const void* key)
{
    unsigned char* str = *(unsigned char**)(size_t)key;
    
    size_t hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

size_t hash_uint(const void* key) 
{
    size_t x = *(size_t*)key;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    return (x >> 16) ^ x;
}