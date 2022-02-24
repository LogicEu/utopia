#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/*************************
 -> Generic Stack Array <- 
*************************/

ustack_t stack(const size_t size, const size_t bytes)
{
    ustack_t stack;
    stack.size = size;
    stack.bytes = bytes;
    stack.used = 0;
    stack.data = calloc(size, bytes);
    return stack;
}

ustack_t* stack_new(const size_t size, const size_t bytes)
{
    ustack_t* stack = malloc(sizeof(ustack_t));
    stack->size = size;
    stack->bytes = bytes;
    stack->used = 0;
    stack->data = calloc(size, bytes);
    return stack;
}

size_t stack_is_empty(const ustack_t* restrict stack)
{
    return stack->used == 0;
}

size_t stack_is_full(const ustack_t* restrict stack)
{
    return stack->used == stack->size;
}

void* stack_index(const ustack_t* restrict stack, const size_t index)
{
    return _stack_index(stack, index);
}

void stack_resize(ustack_t* stack, const size_t size)
{
    if (size < stack->used) return;
    stack->size = size;
    stack->data = realloc(stack->data, stack->size * stack->bytes);
}

void stack_cut(ustack_t* stack)
{
    stack->size = stack->used;
    stack->data = realloc(stack->data, stack->size * stack->bytes);
}

void stack_push(ustack_t* stack, const void* data)
{
    if (!stack->data) stack->data = calloc(stack->size, stack->used);
    if (stack->used == stack->size) stack_resize(stack, stack->size * 2);
    memcpy(_stack_index(stack, stack->used++), data, stack->bytes);
}

void* stack_pop(ustack_t* stack)
{
    if (stack->used == 0) return NULL;
    return _stack_index(stack, --stack->used);
}

void* stack_peek(const ustack_t* restrict stack)
{
    if (stack->used == 0) return NULL;
    return _stack_index(stack, stack->used);
}

void stack_free(ustack_t* stack)
{
    if (!stack || !stack->data) return;
    free(stack->data);
    stack->data = NULL;
    stack->used = 0;
}

void stack_destroy(ustack_t* stack)
{
    if (!stack) return;
    stack_free(stack);
    free(stack);
}
