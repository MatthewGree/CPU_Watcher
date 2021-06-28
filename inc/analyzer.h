#ifndef ANALYZER_H
#define ANALYZER_H

#include <logger.h>
#include <program_state.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>


typedef struct analyzer analyzer;

analyzer *analyzer_create(logger *logger);
void analyzer_destroy(analyzer *analyzer);
bool analyzer_setOutput(analyzer *analyzer, queue *output);
queue* analyzer_getInput(analyzer *analyzer);
int analyzer_createThread(analyzer *analyzer, thrd_t *thread);

#endif
