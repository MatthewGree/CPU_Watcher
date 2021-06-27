#ifndef QUEUE_STRING_H
#define QUEUE_STRING_H

#include <stddef.h>

typedef struct queue_string queue_string;

queue_string *queue_string_create(unsigned int num);
void queue_string_destroy(queue_string *ptr);
char *queue_string_dequeue(queue_string *ptr);
void queue_string_enqueue(queue_string *ptr, char *string);

#endif
