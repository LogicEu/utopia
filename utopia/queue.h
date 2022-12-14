#ifndef UTOPIA_QUEUE_H
#define UTOPIA_QUEUE_H

#ifdef _cplusplus
extern "C" {
#endif

#include <utopia/types.h>

typedef struct queue {
    void* data;
    size_t bytes;
    size_t capacity;
    size_t rear;
    size_t front;
} queue_t;

#define _queue_index(queue, i) ((char*)(queue)->data + (i) * (queue)->bytes)

queue_t queue_create(const size_t bytes);
queue_t queue_reserve(const size_t bytes, const size_t reserve);
queue_t queue_copy(const queue_t* queue);
queue_t queue_move(queue_t* queue);
void queue_push(queue_t* queue, const void* data);
void* queue_index(const queue_t* queue, const size_t index);
void* queue_peek(const queue_t* queue);
void* queue_pop(queue_t* queue);
void queue_resize(queue_t* queue, const size_t size);
void queue_cut(queue_t* queue);
size_t queue_bytes(const queue_t* queue);
size_t queue_size(const queue_t* queue);
size_t queue_capacity(const queue_t* queue);
size_t queue_rear(const queue_t* queue);
size_t queue_front(const queue_t* queue);
void queue_clear(queue_t* stack);
void queue_free(queue_t* queue);

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_QUEUE_H */
