#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct queue queue;
// unit size - if we enqueue a pointer, then unit size is sizeof(*pointer);
queue *queue_create(unsigned int elementCount, size_t unitSize);
void queue_destroy(queue *ptr);
//blocks until there is an element to dequeue, then returns it
//dequeued pointer needs to be freed
void *queue_dequeue(queue *ptr);
// same as queue_dequeue but blocks only for nanoSeconds ns
// and stores in *success if the dequeue operation succeeded or not
void *queue_timedDequeue(queue *ptr, long nanoSeconds, bool *success);
// blocks until there is place in queue, then enqueues the pointer to
// (*elem)'s copy
void queue_enqueue(queue *ptr, const void *elem, size_t unitCount,
                   size_t additionalSize);

#endif
