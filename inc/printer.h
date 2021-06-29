#ifndef PRINTER_H
#define PRINTER_H

#include <logger.h>
#include <queue.h>
#include <stdbool.h>
#include <threads.h>

/*
 * struct which takes cpuLoads from
 * input and converts it's data to string and then prints it.
 * sends bool* to output every repetition of main thread loop
 */
typedef struct printer printer;

printer *printer_create(logger *logger);
void printer_destroy(printer *printer);
// input accepts cpuLoads from common.h
queue *printer_getInput(printer *printer);
// output should accept bool*
bool printer_setOutput(printer *printer, queue *output);
int printer_createThread(printer *printer, thrd_t *thread);

#endif
