#include <queue_string.h>
#include <semaphore.h>
#include <stdlib.h>
#include <threads.h>
#include <string.h>
#include <stdio.h>

struct queue_string {
  mtx_t mutex; // used to allow only one enqueue or dequeue operation at the
               // same time
  sem_t semaphore_dequeue; // used to keep track of number of elements in queue
  sem_t semaphore_enqueue; // and allow exclusive access to each of them
  unsigned int capacity;
  unsigned int curSize;
  char* baseArray[]; //FAM
};

queue_string *queue_string_create(unsigned int num) {
  queue_string *toReturn = malloc(sizeof(queue_string) + num * sizeof(char*));

  if (toReturn) {
    toReturn->capacity = num;
    toReturn->curSize = 0;
    mtx_init(&(toReturn->mutex), mtx_plain);
    sem_init(&toReturn->semaphore_dequeue, 0, toReturn->curSize);
    sem_init(&toReturn->semaphore_enqueue, 0, num);
  }
  return toReturn;
}

void queue_string_destroy(queue_string *ptr) {
  mtx_destroy(&ptr->mutex);
  sem_destroy(&ptr->semaphore_dequeue);
  sem_destroy(&ptr->semaphore_enqueue);
  free(ptr);
}

void queue_string_enqueue(queue_string *ptr, char *string) {
  char *copy = 0;
  if (string) {
    copy = malloc(sizeof(char) * (strlen(string) + 1));
    if (!copy) {
      perror("QUEUE: malloc not working");
    }
    strcpy(copy, string);
  }
  sem_wait(&ptr->semaphore_enqueue);
  mtx_lock(&ptr->mutex);
  ptr->baseArray[ptr->curSize] = copy;
  ptr->curSize++;
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_dequeue);
}

char *queue_string_dequeue(queue_string *ptr) {
  sem_wait(&ptr->semaphore_dequeue);
  mtx_lock(&ptr->mutex);
  char *toReturn = ptr->baseArray[0];
  ptr->curSize--;
  memmove(ptr->baseArray, &ptr->baseArray[1], sizeof(char*) * ptr->curSize);
  mtx_unlock(&ptr->mutex);
  sem_post(&ptr->semaphore_enqueue);
  return toReturn;
}
