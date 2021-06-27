#ifndef LOGGER_H
#define LOGGER_H

#include <queue_string.h>
#include <threads.h>

typedef struct logger logger;


logger *logger_create(unsigned int queueCapacity, char *filename);
void logger_destroy(logger *logger);
int logger_createThread(thrd_t *thread, logger *logger);
void printLog(logger *logger, char *msg);
queue_string* logger_getInput(logger *logger);

#endif
