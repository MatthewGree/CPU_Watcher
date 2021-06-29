#ifndef ANALYZER_H
#define ANALYZER_H

#include <logger.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>

// analyzer's thread assumes that it will receive a proper
// part of /proc/stat in the form of char* on analyzer's input
// sends to output analyzed data in form of cpuLoads
typedef struct analyzer analyzer;

analyzer *analyzer_create(logger *logger);
void analyzer_destroy(analyzer *analyzer);
// output should accept cpuLoads* from common.h
bool analyzer_setOutput(analyzer *analyzer, queue *output);
// input accepts strings
queue *analyzer_getInput(analyzer *analyzer);
int analyzer_createThread(analyzer *analyzer, thrd_t *thread);

#endif
