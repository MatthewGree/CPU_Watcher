#include <logger.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct logger {
  queue *input;
  char *filename;
};

logger *logger_create(unsigned int queueCapacity, char *filename) {
  logger *toReturn = malloc(sizeof(logger));
  if (toReturn) {
    toReturn->input = queue_create(queueCapacity, sizeof(char));
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
    toReturn->filename = filename;
  }
  return toReturn;
}

void logger_destroy(logger *logger) {
  queue_destroy(logger->input);
  free(logger);
}

static int logger_runLogger(void *logger_void) {
  logger *logger = logger_void;
  FILE *file = fopen(logger->filename, "w");
  if (!file) {
    perror("Logger unable to open file, exiting...\n");
    return 1;
  }
  while (true) {
    char *toPrint = queue_dequeue(logger->input);
    if (!toPrint) {
      fprintf(file, "LOGGER: received null, exiting\n");
      break;
    }
    fprintf(file, "%s\n", toPrint);
    free(toPrint);
  }
  fclose(file);
  thrd_exit(0);
}

int logger_createThread(logger *logger, thrd_t *thread) {
  return thrd_create(thread, &logger_runLogger, logger);
}

void logger_printLog(logger *logger, char *msg) {
  if (msg) {
    queue_enqueue(logger->input, msg, strlen(msg) + 1, 0);
  } else {
    queue_enqueue(logger->input, msg, 0, 0);
  }
}

queue *logger_getInput(logger *logger) { return logger->input; }
