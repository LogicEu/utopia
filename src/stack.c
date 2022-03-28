#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/*************************
 -> Generic Stack Array <- 
*************************/

ustack_t stack_create(const size_t bytes)
{
    ustack_t stack;
    stack.data = NULL;
    stack.bytes = bytes;
    stack.capacity = 0;
    stack.used = 0;
    return stack;
}

ustack_t stack_reserve(const size_t bytes, const size_t reserve)
{
    ustack_t stack;
    stack.data = malloc(reserve * bytes);
    stack.bytes = bytes;
    stack.capacity = reserve;
    stack.used = 0;
    return stack;
}

ustack_t stack_copy(const ustack_t* restrict stack)
{
    ustack_t ret;
    ret.data = malloc(stack->capacity * stack->bytes);
    ret.bytes = stack->bytes;
    ret.capacity = stack->capacity;
    ret.used = stack->used;
    memcpy(ret.data, stack->data, ret.used * ret.bytes);
    return ret;
}

void stack_push(ustack_t* restrict stack, const void* restrict data)
{
    if (stack->used >= stack->capacity) {
        stack->capacity = stack->capacity * !!stack->capacity * 2 + !stack->capacity;
        stack->data = !stack->data ? malloc(stack->capacity * stack->bytes) : realloc(stack->data, stack->capacity * stack->bytes);
    }
    memcpy(_array_index(stack, stack->used++), data, stack->bytes);
}

void* stack_pop(ustack_t* restrict stack)
{
    return !stack->used ? NULL : _array_index(stack, --stack->used);
}

void* stack_peek(const ustack_t* restrict stack)
{
    return !stack->used ? NULL : _array_index(stack, stack->used - 1);
}

void* stack_index(const ustack_t* restrict stack, const size_t index)
{
    return _array_index(stack, index);
}

void stack_resize(ustack_t* restrict stack, const size_t size)
{
    stack->capacity = size * (size >= stack->used) + stack->used * (stack->used > size) + !size;
    stack->data = !stack->data ? malloc(stack->capacity * stack->bytes) : realloc(stack->data, stack->capacity * stack->bytes);
}

void stack_cut(ustack_t* restrict stack)
{
    if (!stack->used) return;
    stack->capacity = stack->used;
    stack->data = realloc(stack->data, stack->capacity * stack->bytes);
}

void stack_clear(ustack_t* restrict stack)
{
    stack->used = 0;
}

void stack_free(ustack_t* restrict stack)
{
    if (stack->data) free(stack->data);
    stack->data = NULL;
    stack->capacity = 0;
    stack->used = 0;
}
