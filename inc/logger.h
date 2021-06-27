#ifndef LOGGER_H
#define LOGGER_H

#include <queue_string.h>
#include <threads.h>

typedef struct logger logger;

struct logger {
  queue_string *input;
  char *filename;
};

logger *logger_create(unsigned int queueCapacity, char *filename);
void logger_destroy(logger *logger);
int logger_createThread(thrd_t *thread, logger *logger);
void printLog(logger *logger, char *msg);

#endif
