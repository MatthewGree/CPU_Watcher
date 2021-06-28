#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

typedef struct queue queue;

queue *queue_create(unsigned int elementCount, size_t unitSize);
void queue_destroy(queue *ptr);
void *queue_dequeue(queue *ptr);
void queue_enqueue(queue *ptr, const void *elem, size_t unitCount, size_t additionalSize);

#endif
