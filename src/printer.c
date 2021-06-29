#include <common.h>
#include <printer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time_helper.h>

#define PRINTER_QUEUE_SIZE 1

struct printer {
  queue *input;
  queue *output;
  logger *logger;
};

printer *printer_create(logger *logger) {
  if (!logger) {
    return 0;
  }
  printer *toReturn = malloc(sizeof(printer));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->output = 0;
    toReturn->input = queue_create(PRINTER_QUEUE_SIZE, sizeof(cpuLoads));
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
  }
  return toReturn;
}

void printer_destroy(printer *printer) {
  if (printer) {
    queue_destroy(printer->input);
    free(printer);
  }
}

queue *printer_getInput(printer *printer) { return printer->input; }
bool printer_setOutput(printer *printer, queue *output) {
  if (!printer || printer->output || !output) {
    return false;
  } else {
    printer->output = output;
    return true;
  }
}

static inline void clearScreen() { system("clear"); }

static int printer_runPrinter(void *printer_void) {
  printer *printer = printer_void;
  while (true) {
    long startStamp = time_helper_nanosecondsTimeStamp();
    cpuLoads *loads = queue_dequeue(printer->input);
    if (!loads) {
      logger_printLog(printer->logger, "PRINTER: Received null, exiting");
      break;
    }
    logger_printLog(printer->logger, "PRINTER: Received loads, printing");
    clearScreen();
    for (size_t i = 0; i < loads->size; i++) {
      if (i == 0) {
        printf("CPU: %.2lf%%\n", loads->loads[i]);
      } else {
        printf("CORE: %zu - %.2lf%%\n", i, loads->loads[i]);
      }
    }
    cpuLoads_destroy(loads);
    queue_enqueue(printer->output, &(bool){true}, 1, 0);
    long diff = time_helper_nanosecondsTimeStamp() - startStamp;
    long toSleep = SEC_TO_NS(1) - diff;
    if (toSleep > 0) {
      thrd_sleep(&(struct timespec){.tv_sec = NS_DIV_SEC(toSleep),
                                    .tv_nsec = NS_MOD_SEC(toSleep)},
                 0);
    }
  }
  thrd_exit(0);
}

int printer_createThread(printer *printer, thrd_t *thread) {
  if (printer && printer->output) {
    return thrd_create(thread, printer_runPrinter, printer);
  } else {
    return thrd_error;
  }
}
