#ifndef UTOPIA_QUEUE_H
#define UTOPIA_QUEUE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

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

/******************
Generic Queue Array
*******************/

#ifdef UTOPIA_QUEUE_IMPL

#ifndef USTDLIB_H 
#define USTDLIB_H <stdlib.h>
#endif

#ifndef USTRING_H 
#define USTRING_H <string.h>
#endif

#include USTDLIB_H
#include USTRING_H

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

#endif /* UTOPIA_QUEUE_IMPL */

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_QUEUE_H */
