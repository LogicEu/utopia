#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue_create(const size_t bytes)
{
    queue_t queue;
    queue.data = NULL;
    queue.bytes = bytes;
    queue.size = 0;
    queue.used = 0;
    queue.front = 0;
    queue.rear = 0;
    return queue;
}

queue_t queue_reserve(const size_t bytes, const size_t reserve)
{
    queue_t queue;
    queue.data = malloc(bytes * reserve);
    queue.bytes = bytes;
    queue.size = reserve;
    queue.used = 0;
    queue.front = 0;
    queue.rear = reserve - 1;
    return queue;
}

queue_t queue_copy(const queue_t* restrict queue)
{
    queue_t ret;
    ret.data = malloc(queue->bytes * queue->size);
    ret.bytes = queue->bytes;
    ret.size = queue->size;
    ret.used = queue->used;
    ret.front = queue->front;
    ret.rear = queue->rear;
    memcpy(ret.data, queue->data, ret.bytes * ret.used);
    return ret;
}

void queue_push(queue_t* restrict queue, const void* restrict data)
{
    if (queue->used >= queue->size) {
        queue->size = queue->size * !!queue->size * 2 + !queue->size;
        queue->data = !queue->data ? malloc(queue->size * queue->bytes) : realloc(queue->data, queue->size * queue->bytes);
    }

    queue->rear = (queue->rear + 1) % queue->size;
    memcpy(_array_index(queue, queue->rear), data, queue->bytes);
    ++queue->used;
}

void queue_resize(queue_t* queue, const size_t size)
{
    queue->size = size * (size >= queue->used) + queue->used * (queue->used > size) + !size;
    queue->data = !queue->data ? malloc(queue->size * queue->bytes) : realloc(queue->data, queue->size * queue->bytes);
}

void queue_cut(queue_t* queue)
{
    if (!queue->used) return;
    queue->size = queue->used;
    queue->data = realloc(queue->data, queue->size * queue->bytes);
}

void* queue_pop(queue_t* restrict queue)
{
    if (!queue->used) return NULL;
    void* ptr = _array_index(queue, queue->front);
    queue->front = (queue->front + 1) % queue->size;
    --queue->used;
    return ptr;
}

void* queue_peek(const queue_t* restrict queue)
{
    return !queue->used ? NULL : _array_index(queue, queue->front);
}

void* queue_index(const queue_t* restrict queue, const size_t index)
{
    return _array_index(queue, index);
}

void queue_clear(queue_t* restrict queue)
{
    queue->used = 0;
    queue->front = 0;
    queue->rear = queue->size - 1;
}

void queue_free(queue_t* restrict queue)
{
    if (queue->data) free(queue->data);
    queue->data = NULL;
    queue->size = 0;
    queue->used = 0;
    queue->front = 0;
    queue->rear = 0;
}

