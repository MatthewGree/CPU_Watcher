#include <analyzer.h>
#include <logger.h>
#include <printer.h>
#include <program_state.h>
#include <queue.h>
#include <reader.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <stdlib.h>

#define NUM_OF_THREADS 4
#define NUM_OF_OBJECTS 5

int main() {

  // setting up objects
  logger *logger = logger_create(1, "output");
  program_state *state = program_state_create(true);
  reader *reader = reader_create(logger, state);
  analyzer *analyzer = analyzer_create(logger);
  printer *printer = printer_create(logger);
  if (!logger || !state || !reader || !analyzer || !printer) {
    printf("ERROR: Some objects weren't created, exiting...");
    logger_destroy(logger);
    program_state_destroy(state);
    reader_destroy(reader);
    analyzer_destroy(analyzer);
    printer_destroy(printer);
    thrd_exit(0);
  }
  reader_setOutput(reader, analyzer_getInput(analyzer));
  analyzer_setOutput(analyzer, printer_getInput(printer));

  // setting up threads
  thrd_t loggerThread;
  thrd_t readerThread;
  thrd_t analyzerThread;
  thrd_t printerThread;
  printf("starting logger thread\n");
  logger_createThread(logger, &loggerThread);
  logger_printLog(logger, "starting reader thread");
  reader_createThread(reader, &readerThread);
  logger_printLog(logger, "starting analyzer thread");
  analyzer_createThread(analyzer, &analyzerThread);
  logger_printLog(logger, "starting printer thread");
  printer_createThread(printer, &printerThread);

  // waiting for program to finish
  thrd_sleep(&(struct timespec){30, 0}, 0);

  // finishing program
  logger_printLog(logger, "MAIN: finishing program");
  bool stop = false;
  atomic_store(&state->isRunning, stop);

  // joining threads
  thrd_join(readerThread, 0);
  logger_printLog(logger, "MAIN: reader came back");
  thrd_join(analyzerThread, 0);
  logger_printLog(logger, " MAIN: analyzer came back");
  thrd_join(printerThread, 0);
  logger_printLog(logger, "MAIN: printer came back");
  logger_printLog(logger, 0);
  thrd_join(loggerThread, 0);

  // all threads joined, destroying objects
  reader_destroy(reader);
  logger_destroy(logger);
  program_state_destroy(state);
  analyzer_destroy(analyzer);
  printer_destroy(printer);
  return 0;
}
