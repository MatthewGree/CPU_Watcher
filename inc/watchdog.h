#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <logger.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>

typedef struct watchdog watchdog;

watchdog *watchdog_create(logger *logger);
void watchdog_destroy(watchdog *watchdog);
// input accepts bool*
queue *watchdog_getInput(watchdog *watchdog);
int watchdog_createThread(watchdog *watchdog, thrd_t *thread);

#endif
