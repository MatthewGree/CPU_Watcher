#ifndef PRINTER_H
#define PRINTER_H

#include <logger.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>

typedef struct printer printer;

printer *printer_create(logger *logger);
void printer_destroy(printer *printer);
// input accepts cpuLoads from common.h
queue *printer_getInput(printer *printer);
int printer_createThread(printer *printer, thrd_t *thread);

#endif
