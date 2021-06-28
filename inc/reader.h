#ifndef READER_H
#define READER_H

#include <logger.h>
#include <program_state.h>
#include <queue_string.h>
#include <stdbool.h>
#include <threads.h>

typedef struct reader reader;

reader *reader_create(logger *logger, program_state *state);
void reader_destroy(reader *reader);
bool reader_setOutput(reader *reader, queue_string *output);
int reader_createThread(reader *reader, thrd_t *thread);

#endif
