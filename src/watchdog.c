#include <stdlib.h>
#include <time_helper.h>
#include <watchdog.h>

#define WATCHDOG_MAX_SECONDS 2
#define WATCHDOG_QUEUE_SIZE 10

struct watchdog {
  queue *input;
  logger *logger;
};

watchdog *watchdog_create(logger *logger) {
  if (!logger) {
    return 0;
  }
  watchdog *toReturn = malloc(sizeof(watchdog));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->input = queue_create(WATCHDOG_QUEUE_SIZE, sizeof(bool));
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
  }
  return toReturn;
}

void watchdog_destroy(watchdog *watchdog) {
  if (watchdog) {
    queue_destroy(watchdog->input);
    free(watchdog);
  }
}

queue *watchdog_getInput(watchdog *watchdog) { return watchdog->input; }

static int watchdog_runWatchdog(void *watchdog_void) {
  watchdog *watchdog = watchdog_void;
  while (true) {
    bool success = false;
    bool *response = queue_timedDequeue(
        watchdog->input, SEC_TO_NS(WATCHDOG_MAX_SECONDS), &success);
    if (!success) {
      logger_printLog(watchdog->logger, "WATCHDOG: program hanging, exiting");
      exit(1);
    }
    if (!response) {
      break;
    }
    free(response);
  }
  thrd_exit(0);
}

int watchdog_createThread(watchdog *watchdog, thrd_t *thread) {
  if (watchdog) {
    return thrd_create(thread, watchdog_runWatchdog, watchdog);
  } else {
    return thrd_error;
  }
}
