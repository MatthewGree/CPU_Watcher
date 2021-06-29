#ifndef LOGGER_H
#define LOGGER_H

#include <queue.h>
#include <threads.h>


/*
 * prints every char* from input to filename
 */
typedef struct logger logger;

logger *logger_create(unsigned int queueCapacity, char *filename);
void logger_destroy(logger *logger);
int logger_createThread(logger* logger, thrd_t *thread);
void logger_printLog(logger *logger, char *msg);
queue *logger_getInput(logger *logger);

#endif
