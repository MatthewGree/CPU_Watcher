#include <queue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time_helper.h>

struct queue {
  mtx_t mutex; // used to allow only one enqueue or dequeue operation at the
               // same time
  sem_t semaphore_dequeue; // used to keep track of number of elements in queue
  sem_t semaphore_enqueue; // and allow exclusive access to each of them
  unsigned int capacity;
  unsigned int curSize;
  size_t unitSize;
  void *baseArray[]; // FAM
};

queue *queue_create(unsigned int elementCount, size_t unitSize) {
  queue *toReturn = malloc(sizeof(queue) + elementCount * sizeof(void *));

  if (toReturn) {
    toReturn->capacity = elementCount;
    toReturn->curSize = 0;
    toReturn->unitSize = unitSize;
    mtx_init(&(toReturn->mutex), mtx_plain);
    sem_init(&toReturn->semaphore_dequeue, 0, toReturn->curSize);
    sem_init(&toReturn->semaphore_enqueue, 0, elementCount);
  }
  return toReturn;
}

void queue_destroy(queue *ptr) {
  if (ptr) {
    mtx_destroy(&ptr->mutex);
    sem_destroy(&ptr->semaphore_dequeue);
    sem_destroy(&ptr->semaphore_enqueue);
    free(ptr);
  }
}

void queue_enqueue(queue *ptr, const void *elem, size_t unit_count,
                   size_t additionalSize) {
  char *copy = 0;
  if (elem) {
    size_t copySize = ptr->unitSize * unit_count + additionalSize;
    copy = malloc(copySize);
    if (!copy) {
      perror("QUEUE: malloc not working");
    } else {
      memcpy(copy, elem, copySize);
    }
  }
  sem_wait(&ptr->semaphore_enqueue);
  mtx_lock(&ptr->mutex);
  ptr->baseArray[ptr->curSize] = copy;
  ptr->curSize++;
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_dequeue);
}

static void *queue_mutexDequeue(queue *ptr) {
  mtx_lock(&ptr->mutex);
  void *toReturn = ptr->baseArray[0];
  ptr->curSize--;
  memmove(ptr->baseArray, &ptr->baseArray[1], sizeof(void *) * ptr->curSize);
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_enqueue);
  return toReturn;
}

void *queue_dequeue(queue *ptr) {
  if (sem_wait(&ptr->semaphore_dequeue) != -1) {
    return queue_mutexDequeue(ptr);
  } else {
    return 0;
  }
}

void *queue_timedDequeue(queue *ptr, long nanoseconds, bool *success) {
  struct timespec t;
  timespec_get(&t, TIME_UTC);
  int result = sem_timedwait(
      &ptr->semaphore_dequeue,
      &(struct timespec){.tv_sec = t.tv_sec + NS_DIV_SEC(nanoseconds),
                         .tv_nsec = t.tv_nsec + NS_MOD_SEC(nanoseconds)});
  if (result == -1) {
    *success = false;
    return 0;
  }
  *success = true;
  return queue_mutexDequeue(ptr);
}
