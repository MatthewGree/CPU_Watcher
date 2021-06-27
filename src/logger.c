#include <logger.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct logger {
  queue_string *input;
  char *filename;
};

logger *logger_create(unsigned int queueCapacity, char *filename) {
  logger *toReturn = malloc(sizeof(logger));
  if (toReturn) {
    toReturn->input = queue_string_create(queueCapacity);
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
    toReturn->filename = filename;
  }
  return toReturn;
}

void logger_destroy(logger *logger) {
  queue_string_destroy(logger->input);
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
    char *toPrint = queue_string_dequeue(logger->input);
    if (!toPrint) {
      fprintf(file, "Logger finished job\n");
      break;
    }
    fprintf(file, "%s\n", toPrint);
  }
  fclose(file);
  thrd_exit(0);
}

int logger_createThread(thrd_t *thread, logger *logger) {
  return thrd_create(thread, &logger_runLogger, logger);
}

void logger_printLog(logger *logger, char *msg) {
  if (logger->input) {
    queue_string_enqueue(logger->input, msg);
  }
}

queue_string *logger_getInput(logger *logger) { return logger->input; }
