#include <common.h>
#include <printer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PRINTER_QUEUE_SIZE 1
#define SEC_TO_NS(s) ((s)*1000000000)
#define NS_DIV_SEC(ns) (ns / 1000000000)
#define NS_MOD_SEC(ns) (ns % 1000000000)

struct printer {
  queue *input;
  logger *logger;
};

printer *printer_create(logger *logger) {
  printer *toReturn = malloc(sizeof(printer));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->input = queue_create(PRINTER_QUEUE_SIZE, sizeof(cpuLoads));
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
  }
  return toReturn;
}

void printer_destroy(printer *printer) {
  queue_destroy(printer->input);
  free(printer);
}

queue *printer_getInput(printer *printer) { return printer->input; }

static long printer_nanosecondsTimeStamp() {
  struct timespec t;
  timespec_get(&t, TIME_UTC);
  return SEC_TO_NS(t.tv_sec) + t.tv_nsec;
}

static inline void clearScreen() { system("clear"); }

static int printer_runPrinter(void *printer_void) {
  printer *printer = printer_void;
  while (true) {
    long startStamp = printer_nanosecondsTimeStamp();
    cpuLoads *loads = queue_dequeue(printer->input);
    if (!loads) {
      logger_printLog(printer->logger, "PRINTER: Received null, exiting");
      break;
    }
    clearScreen();
    for (size_t i = 0; i < loads->size; i++) {
      if (i == 0) {
        printf("CPU: %.2lf%%\n", loads->loads[i]);
      } else {
        printf("CORE: %zu - %.2lf%%\n", i, loads->loads[i]);
      }
    }
    long diff = printer_nanosecondsTimeStamp() - startStamp;
    long toSleep = SEC_TO_NS(1) - diff;
    if (toSleep > 0) {
      thrd_sleep(&(struct timespec){.tv_sec = NS_DIV_SEC(toSleep),
                                    .tv_nsec = NS_MOD_SEC(toSleep)},
                 &(struct timespec){.tv_sec = 0, .tv_nsec = 0});
    }
  }
  thrd_exit(0);
}

int printer_createThread(printer *printer, thrd_t *thread) {
  if (printer) {
    return thrd_create(thread, printer_runPrinter, printer);
  } else {
    return thrd_error;
  }
}
