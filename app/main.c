#include <analyzer.h>
#include <logger.h>
#include <printer.h>
#include <program_state.h>
#include <reader.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <watchdog.h>

#define LOG_FILE "log_file.txt"
#define LOGGER_CAPACITY 20

static sem_t exitingSem;
static atomic_bool intFlag = false;

static void sigIntHandler(int signum) {
  if (atomic_load(&intFlag)) {
    sem_post(&exitingSem);
    atomic_store(&intFlag, false);
  }

  if (signum == SIGINT) {
    signal(signum, sigIntHandler);
  }
}

int main() {
  // setting up SIGTERM handling
  if (sem_init(&exitingSem, 0, 0) == -1) {
    printf("ERROR: semaphore not initialized, exiting");
    thrd_exit(0);
  }
  if (signal(SIGINT, sigIntHandler) == SIG_ERR) {
    printf("ERROR: signal handler not initialized, exiting");
    thrd_exit(0);
  }
  atomic_store(&intFlag, true);
  // setting up objects
  logger *logger = logger_create(LOGGER_CAPACITY, LOG_FILE);
  program_state *state = program_state_create(true);
  reader *reader = reader_create(logger, state);
  analyzer *analyzer = analyzer_create(logger);
  printer *printer = printer_create(logger);
  watchdog *watchdog = watchdog_create(logger);
  if (!logger || !state || !reader || !analyzer || !printer || !watchdog) {
    printf("ERROR: Some objects weren't created, exiting...");
    logger_destroy(logger);
    program_state_destroy(state);
    reader_destroy(reader);
    analyzer_destroy(analyzer);
    printer_destroy(printer);
    watchdog_destroy(watchdog);
    thrd_exit(0);
  }
  reader_setOutput(reader, analyzer_getInput(analyzer));
  analyzer_setOutput(analyzer, printer_getInput(printer));
  printer_setOutput(printer, watchdog_getInput(watchdog));

  // setting up threads
  thrd_t loggerThread;
  thrd_t readerThread;
  thrd_t analyzerThread;
  thrd_t printerThread;
  thrd_t watchdogThread;
  printf("starting logger thread\n");
  logger_createThread(logger, &loggerThread);
  logger_printLog(logger, "starting reader thread");
  reader_createThread(reader, &readerThread);
  logger_printLog(logger, "starting analyzer thread");
  analyzer_createThread(analyzer, &analyzerThread);
  logger_printLog(logger, "starting printer thread");
  printer_createThread(printer, &printerThread);
  logger_printLog(logger, "starting watchdog thread");
  watchdog_createThread(watchdog, &watchdogThread);

  // waiting for program to finish
  sem_wait(&exitingSem);
  sem_destroy(&exitingSem);

  // finishing program
  logger_printLog(logger, "MAIN: finishing program");
  atomic_store(&state->isRunning, false);

  // joining threads
  thrd_join(readerThread, 0);
  logger_printLog(logger, "MAIN: reader came back");
  thrd_join(analyzerThread, 0);
  logger_printLog(logger, " MAIN: analyzer came back");
  thrd_join(printerThread, 0);
  logger_printLog(logger, "MAIN: printer came back");
  thrd_join(watchdogThread, 0);
  logger_printLog(logger, "MAIN: watchdog came back");
  logger_printLog(logger, 0);
  thrd_join(loggerThread, 0);

  // all threads joined, destroying objects
  reader_destroy(reader);
  logger_destroy(logger);
  program_state_destroy(state);
  analyzer_destroy(analyzer);
  printer_destroy(printer);
  watchdog_destroy(watchdog);
  printf("PROGRAM EXITED\n");
  return 0;
}
