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

#ifndef UTOPIA_QUEUE_H
#define UTOPIA_QUEUE_H

/*=======================================================
**************  UTOPIA UTILITY LIBRARY   ****************
Simple and easy generic containers & data structures in C 
================================== @Eugenio Arteaga A. */

/******************
Generic Queue Array
*******************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USTDDEF_H
#define USTDDEF_H <stddef.h>
#endif

#include USTDDEF_H

struct queue {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t rear;
    size_t front;
};

#define _queue_index(queue, i) ((char*)(queue)->data + (i) * (queue)->bytes)

struct queue queue_create(const size_t bytes);
struct queue queue_reserve(const size_t bytes, const size_t reserve);
struct queue queue_copy(const struct queue* queue);
struct queue queue_move(struct queue* queue);
void queue_push(struct queue* queue, const void* data);
void* queue_index(const struct queue* queue, const size_t index);
void* queue_peek(const struct queue* queue);
void* queue_pop(struct queue* queue);
void queue_resize(struct queue* queue, const size_t size);
void queue_cut(struct queue* queue);
size_t queue_bytes(const struct queue* queue);
size_t queue_size(const struct queue* queue);
size_t queue_capacity(const struct queue* queue);
size_t queue_rear(const struct queue* queue);
size_t queue_front(const struct queue* queue);
void queue_clear(struct queue* stack);
void queue_free(struct queue* queue);

#ifdef __cplusplus
}
#endif
#endif /* UTOPIA_QUEUE_H */

#ifdef UTOPIA_IMPLEMENTATION

#ifndef UTOPIA_QUEUE_IMPLEMENTED
#define UTOPIA_QUEUE_IMPLEMENTED

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

/******************
Generic Queue Array
*******************/

struct queue queue_create(const size_t bytes)
{
    struct queue queue;
    queue.bytes = bytes + !bytes;
    queue.data = NULL;
    queue.capacity = 0;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

struct queue queue_reserve(const size_t bytes, const size_t reserve)
{
    struct queue queue;
    queue.bytes = bytes + !bytes;
    queue.data = reserve ? malloc(queue.bytes * reserve) : NULL;
    queue.capacity = reserve;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

struct queue queue_copy(const struct queue* queue)
{
    struct queue ret;
    ret.data = malloc(queue->bytes * queue->capacity);
    ret.bytes = queue->bytes;
    ret.capacity = queue->capacity;
    ret.front = queue->front;
    ret.rear = queue->rear;
    
    memcpy(ret.data, queue->data, ret.bytes * ret.capacity);
    return ret;
}

struct queue queue_move(struct queue* queue)
{
    struct queue ret;
    ret.data = queue->data;
    ret.bytes = queue->bytes;
    ret.capacity = queue->capacity;
    ret.front = queue->front;
    ret.rear = queue->rear;
    
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = 0;
    queue->data = NULL;

    return ret;
}

void queue_push(struct queue* queue, const void* data)
{
    const size_t size = queue->rear - queue->front;

    if (size >= queue->capacity) {
        queue->capacity = queue->capacity * 2 + !queue->capacity;
        queue->data = realloc(queue->data, queue->capacity * queue->bytes);
    }

    memcpy(_queue_index(queue, queue->rear), data, queue->bytes);
    queue->rear = (queue->rear + 1) % queue->capacity;
}

void queue_resize(struct queue* queue, const size_t size)
{
    const size_t qsize = queue->rear - queue->front;
    queue->capacity = (size > qsize) ? size : qsize;
    queue->data = realloc(queue->data, queue->capacity * queue->bytes);
}

void queue_cut(struct queue* queue)
{
    queue->capacity = queue->rear - queue->front;
    queue->data = realloc(queue->data, queue->capacity * queue->bytes);
}

void* queue_pop(struct queue* queue)
{
    void* ptr;
    if (queue->rear == queue->front) {
        return NULL;
    }

    ptr = _queue_index(queue, queue->front);
    queue->front = (queue->front + 1) % queue->capacity;
    
    return ptr;
}

void* queue_peek(const struct queue* queue)
{
    return queue->rear == queue->front ? NULL : _queue_index(queue, queue->front);
}

void* queue_index(const struct queue* queue, const size_t index)
{
    return _queue_index(queue, index);
}

size_t queue_bytes(const struct queue* queue)
{
    return queue->bytes;
}

size_t queue_size(const struct queue* queue)
{
    return queue->rear >= queue->front ? queue->rear - queue->front : queue->capacity - queue->front + queue->rear;
}

size_t queue_capacity(const struct queue* queue)
{
    return queue->capacity;
}

size_t queue_rear(const struct queue* queue)
{
    return queue->rear;
}

size_t queue_front(const struct queue* queue)
{
    return queue->front;
}

void queue_clear(struct queue* queue)
{
    queue->front = 0;
    queue->rear = 0;
}

void queue_free(struct queue* queue)
{
    if (queue->data) {
        free(queue->data);
        queue->data = NULL;
        queue->capacity = 0;
        queue->front = 0;
        queue->rear = 0;
    }
}

#endif /* UTOPIA_QUEUE_IMPLEMENTED */
#endif /* UTOPIA_IMPLEMENTATION */
