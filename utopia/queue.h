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

#ifdef _cplusplus
}
#endif
#endif /* UTOPIA_QUEUE_H */
