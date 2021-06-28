#ifndef READER_H
#define READER_H

#include <logger.h>
#include <program_state.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>

typedef struct reader reader;

/*
   creates a reader with input queue to which only strings
   should be written
 */
reader *reader_create(logger *logger, program_state *state);
void reader_destroy(reader *reader);
bool reader_setOutput(reader *reader, queue *output);
int reader_createThread(reader *reader, thrd_t *thread);

#endif
