#include <utopia.h>
#include <stdlib.h>
#include <string.h>

/*************************
 -> Generic Queue Array <- 
*************************/

queue_t queue_create(const size_t bytes)
{
    queue_t queue;
    queue.bytes = bytes + !bytes;
    queue.data = NULL;
    queue.capacity = 0;
    queue.size = 0;
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
    queue.size = 0;
    queue.front = 0;
    queue.rear = reserve ? reserve - 1 : 0;
    return queue;
}

queue_t queue_copy(const queue_t* restrict queue)
{
    queue_t ret;
    ret.data = malloc(queue->bytes * queue->capacity);
    ret.bytes = queue->bytes;
    ret.capacity = queue->capacity;
    ret.size = queue->size;
    ret.front = queue->front;
    ret.rear = queue->rear;
    
    memcpy(ret.data, queue->data, ret.bytes * ret.size);
    return ret;
}

queue_t queue_move(queue_t* restrict queue)
{
    queue_t ret;
    ret.data = queue->data;
    ret.bytes = queue->bytes;
    ret.capacity = queue->capacity;
    ret.size = queue->size;
    ret.front = queue->front;
    ret.rear = queue->rear;
    
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = 0;
    queue->data = NULL;

    return ret;
}

void queue_push(queue_t* restrict queue, const void* restrict data)
{
    if (queue->size >= queue->capacity) {
        queue->capacity = queue->capacity * 2 + !queue->capacity;
        queue->data = !queue->data ? malloc(queue->capacity * queue->bytes) : realloc(queue->data, queue->capacity * queue->bytes);
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    memcpy(_array_index(queue, queue->rear), data, queue->bytes);
    ++queue->size;
}

void queue_resize(queue_t* queue, const size_t size)
{
    if (!size) return;
    queue->capacity = (size > queue->size) ? size : queue->size;
    queue->data = !queue->data ? malloc(queue->capacity * queue->bytes) : realloc(queue->data, queue->capacity * queue->bytes);
}

void queue_cut(queue_t* queue)
{
    if (!queue->size) return;
    queue->capacity = queue->size;
    queue->data = realloc(queue->data, queue->capacity * queue->bytes);
}

void* queue_pop(queue_t* restrict queue)
{
    if (!queue->size) return NULL;
    void* ptr = _array_index(queue, queue->front);
    queue->front = (queue->front + 1) % queue->capacity;
    --queue->size;
    return ptr;
}

void* queue_peek(const queue_t* restrict queue)
{
    return !queue->size ? NULL : _array_index(queue, queue->front);
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
    return queue->size;
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
    queue->size = 0;
    queue->front = 0;
    queue->rear = queue->capacity - 1;
}

void queue_free(queue_t* restrict queue)
{
    if (queue->data) free(queue->data);
    queue->data = NULL;
    queue->capacity = 0;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
}
