#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/*************************
 -> Generic Stack Array <- 
*************************/

ustack_t stack(unsigned int size, unsigned int bytes)
{
    ustack_t stack;
    stack.size = size;
    stack.bytes = bytes;
    stack.used = 0;
    stack.data = malloc(size * bytes);
    return stack;
}

ustack_t* stack_new(unsigned int size, unsigned int bytes)
{
    ustack_t* stack = (ustack_t*)malloc(sizeof(ustack_t));
    stack->size = size;
    stack->bytes = bytes;
    stack->used = 0;
    stack->data = malloc(size * bytes);
    return stack;
}

unsigned int stack_is_empty(ustack_t* stack)
{
    return stack->used == 0;
}

unsigned int stack_is_full(ustack_t* stack)
{
    return stack->used == stack->size;
}

void* stack_index(ustack_t* stack, unsigned int index)
{
    return (void*)((char*)stack->data + index * stack->bytes);
}

void stack_resize(ustack_t* stack, unsigned int new_size)
{
    if (new_size < stack->used) return;
    stack->size = new_size;
    stack->data = realloc(stack->data, stack->size * stack->bytes);
}

void stack_cut(ustack_t* stack)
{
    stack->size = stack->used;
    stack->data = realloc(stack->data, stack->size * stack->bytes);
}

void stack_push(ustack_t* stack, void* data)
{
    if (stack->data == NULL) stack->data = malloc(stack->size * stack->used);
    if (stack_is_full(stack)) stack_resize(stack, stack->size * 2);
    memcpy(stack_index(stack, stack->used++), data, stack->bytes);
}

void* stack_pop(ustack_t* stack)
{
    if (stack_is_empty(stack)) return NULL;
    return stack_index(stack, --stack->used);
}

void* stack_peek(ustack_t* stack)
{
    if (stack_is_empty(stack)) return NULL;
    return stack_index(stack, stack->used);
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
    if (stack == NULL) return;
    stack_free(stack);
    free(stack);
}
