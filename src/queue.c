#include <utopia.h>
#include USTDLIB_H
#include USTRING_H

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue_create(const size_t bytes)
{
    queue_t queue;
    queue.bytes = bytes + !bytes;
    queue.data = NULL;
    queue.capacity = 0;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

queue_t queue_reserve(const size_t bytes, const size_t reserve)
{
    queue_t queue;
    queue.bytes = bytes + !bytes;
    queue.data = reserve ? malloc(queue.bytes * reserve) : NULL;
    queue.capacity = reserve;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

queue_t queue_copy(const queue_t* restrict queue)
{
    queue_t ret;
    ret.data = malloc(queue->bytes * queue->capacity);
    ret.bytes = queue->bytes;
    ret.capacity = queue->capacity;
    ret.front = queue->front;
    ret.rear = queue->rear;
    
    memcpy(ret.data, queue->data, ret.bytes * ret.capacity);
    return ret;
}

queue_t queue_move(queue_t* restrict queue)
{
    queue_t ret;
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

void queue_push(queue_t* restrict queue, const void* restrict data)
{
    const size_t size = queue->rear - queue->front;

    if (size >= queue->capacity) {
        queue->capacity = queue->capacity * 2 + !queue->capacity;
        queue->data = realloc(queue->data, queue->capacity * queue->bytes);
    }

    memcpy(_array_index(queue, queue->rear), data, queue->bytes);
    queue->rear = (queue->rear + 1) % queue->capacity;
}

void queue_resize(queue_t* queue, const size_t size)
{
    const size_t qsize = queue->rear - queue->front;
    queue->capacity = (size > qsize) ? size : qsize;
    queue->data = realloc(queue->data, queue->capacity * queue->bytes);
}

void queue_cut(queue_t* queue)
{
    queue->capacity = queue->rear - queue->front;
    queue->data = realloc(queue->data, queue->capacity * queue->bytes);
}

void* queue_pop(queue_t* restrict queue)
{
    if (queue->rear == queue->front) {
        return NULL;
    }

    void* ptr = _array_index(queue, queue->front);
    queue->front = (queue->front + 1) % queue->capacity;
    
    return ptr;
}

void* queue_peek(const queue_t* restrict queue)
{
    return queue->rear == queue->front ? NULL : _array_index(queue, queue->front);
}

void* queue_index(const queue_t* restrict queue, const size_t index)
{
    return _array_index(queue, index);
}

size_t queue_bytes(const queue_t* restrict queue)
{
    return queue->bytes;
}

size_t queue_size(const queue_t* restrict queue)
{
    return queue->rear >= queue->front ? queue->rear - queue->front : queue->capacity - queue->front + queue->rear;
}

size_t queue_capacity(const queue_t* restrict queue)
{
    return queue->capacity;
}

size_t queue_rear(const queue_t* restrict queue)
{
    return queue->rear;
}

size_t queue_front(const queue_t* restrict queue)
{
    return queue->front;
}

void queue_clear(queue_t* restrict queue)
{
    queue->front = 0;
    queue->rear = 0;
}

void queue_free(queue_t* restrict queue)
{
    if (queue->data) {
        free(queue->data);
        queue->data = NULL;
        queue->capacity = 0;
        queue->front = 0;
        queue->rear = 0;
    }
}
