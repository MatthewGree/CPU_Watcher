#include <list_string.h>
#include <queue_string.h>
#include <semaphore.h>
#include <stdlib.h>
#include <threads.h>

struct queue_string {
  mtx_t mutex; // used to allow only one enqueue or dequeue operation at the
               // same time
  sem_t semaphore_dequeue; // used to keep track of number of elements in queue
  sem_t semaphore_enqueue; // and allow exclusive access to each of them
  list_string *baseList;
  unsigned int capacity;
  unsigned int curSize;
};

queue_string *queue_string_create(unsigned int num) {
  queue_string *toReturn = malloc(sizeof(queue_string));

  if (toReturn) {
    toReturn->capacity = num;
    toReturn->curSize = 0;
    toReturn->baseList = list_string_create();
    mtx_init(&(toReturn->mutex), mtx_plain);
    sem_init(&toReturn->semaphore_dequeue, 0, toReturn->curSize);
    sem_init(&toReturn->semaphore_enqueue, 0, num);
  }
  return toReturn;
}

void queue_string_destroy(queue_string *ptr) {
  list_string_destroy(ptr->baseList);
  mtx_destroy(&ptr->mutex);
  sem_destroy(&ptr->semaphore_dequeue);
  sem_destroy(&ptr->semaphore_enqueue);
  free(ptr);
}

void queue_string_enqueue(queue_string *ptr, char *string) {
  sem_wait(&ptr->semaphore_enqueue);
  mtx_lock(&ptr->mutex);
  list_string_append(ptr->baseList, string);
  ptr->curSize++;
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_dequeue);
}

char *queue_string_dequeue(queue_string *ptr) {
  sem_wait(&ptr->semaphore_dequeue);
  mtx_lock(&ptr->mutex);
  char *toReturn;
  list_string_popFirst(ptr->baseList, &toReturn);
  ptr->curSize--;
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_enqueue);
  return toReturn;
}
