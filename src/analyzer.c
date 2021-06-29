#include <analyzer.h>
#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#define ANALYZER_QUEUE_SIZE 1
#define ANALYZER_LINE_TOKEN_LENGTH 11 // from specification of /proc/stat

enum statIndexes {
  User,
  Nice,
  System,
  Idle,
  Iowait,
  Irq,
  Softirq,
  Steal,
  Guest,
  Guest_nice
};

struct analyzer {
  queue *output;
  queue *input;
  logger *logger;
};

analyzer *analyzer_create(logger *logger) {
  if (!logger) {
    return 0;
  }
  analyzer *toReturn = malloc(sizeof(analyzer));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->output = 0;
    toReturn->input = queue_create(ANALYZER_QUEUE_SIZE, sizeof(char));
    if (!toReturn->input) {
      free(toReturn);
      return 0;
    }
  }
  return toReturn;
}
void analyzer_destroy(analyzer *analyzer) {
  if (analyzer) {
    queue_destroy(analyzer->input);
    free(analyzer);
  }
}

bool analyzer_setOutput(analyzer *analyzer, queue *output) {
  if (analyzer->output || !output) {
    return false;
  } else {
    analyzer->output = output;
    return true;
  }
}
queue *analyzer_getInput(analyzer *analyzer) { return analyzer->input; }

noreturn static void analyzer_cleanUp(analyzer *analyzer, int exitCode) {
  queue_enqueue(analyzer->output, 0, 0, 0);
  thrd_exit(exitCode);
}

static size_t analyzer_getNumberOfLines(char *string) {
  size_t count = 0;
  if (string) {
    for (size_t i = 0; string[i] != 0; i++) {
      if (string[i] == '\n') {
        count++;
      }
    }
  }
  return count;
}

static void analyzer_splitString(char *string, char **lines,
                                 size_t numberOfLines, char *delim) {
  char *saveptr = 0;
  size_t index = 0;
  char *line = __strtok_r(string, delim, &saveptr);
  while (line != 0 && index != numberOfLines) {
    lines[index] = line;
    index++;
    line = __strtok_r(0, delim, &saveptr);
  }
  return;
}

static unsigned long analyzer_q_strtoul(char *string) {
  return strtoul(string, &(char *){0}, 10);
}

static void analyzer_calculateActiveAndIdle(char *line, unsigned long *idle,
                                            unsigned long *active) {
  char *tokenizedLine[ANALYZER_LINE_TOKEN_LENGTH];
  analyzer_splitString(line, tokenizedLine, ANALYZER_LINE_TOKEN_LENGTH, " ");
  unsigned long numbers[ANALYZER_LINE_TOKEN_LENGTH - 1];

  for (size_t i = 0; i < ANALYZER_LINE_TOKEN_LENGTH - 1; i++) {
    numbers[i] = analyzer_q_strtoul(tokenizedLine[i + 1]);
  }

  *idle = numbers[Idle] + numbers[Iowait];
  *active = numbers[User] + numbers[Nice] + numbers[System] + numbers[Irq] +
            numbers[Softirq] + numbers[Steal];
}

static double analyzer_calculateLoad(char *prevLine, char *nextLine) {
  unsigned long prevIdle;
  unsigned long prevActive;
  unsigned long nextIdle;
  unsigned long nextActive;
  analyzer_calculateActiveAndIdle(prevLine, &prevIdle, &prevActive);
  analyzer_calculateActiveAndIdle(nextLine, &nextIdle, &nextActive);

  unsigned long prevTotal = prevIdle + prevActive;
  unsigned long nextTotal = nextIdle + nextActive;

  unsigned long diffTotal = nextTotal - prevTotal;
  unsigned long diffIdle = nextIdle - prevIdle;
  return (100.0 * (double)(diffTotal - diffIdle)) / ((double)diffTotal);
}

static int analyzer_runAnalyzer(void *analyzer_void) {
  analyzer *analyzer = analyzer_void;
  logger_printLog(analyzer->logger, "ANALYZER: entering main loop");
  while (true) {
    char *samples = queue_dequeue(analyzer->input);
    if (!samples) {
      logger_printLog(analyzer->logger,
                      "ANALYZER: received null, exiting main loop");
      break;
    }

    char *saveptr = 0;
    char *prev = __strtok_r(samples, COMMON_STATS_DELIMITER, &saveptr);
    char *next = __strtok_r(0, COMMON_STATS_DELIMITER, &saveptr);
    size_t numberOfLines = analyzer_getNumberOfLines(prev);
    if (numberOfLines != analyzer_getNumberOfLines(next)) {
      logger_printLog(analyzer->logger,
                      "ANALYZER: Samples aren't equal, forfeiting");
      free(samples);
      continue;
    }
    char **prevLines = malloc(sizeof(char *) * numberOfLines);
    char **nextLines = malloc(sizeof(char *) * numberOfLines);
    cpuLoads *loads = cpuLoads_create(numberOfLines);

    if (!prevLines || !nextLines || !loads) {
      logger_printLog(analyzer->logger, "ANALYZER: malloc failed, exiting");
      free(samples);
      analyzer_cleanUp(analyzer, 1);
    }

    analyzer_splitString(prev, prevLines, numberOfLines, "\n");
    analyzer_splitString(next, nextLines, numberOfLines, "\n");

    for (size_t i = 0; i < numberOfLines; i++) {
      loads->loads[i] = analyzer_calculateLoad(prevLines[i], nextLines[i]);
    }
    loads->size = numberOfLines;

    logger_printLog(analyzer->logger,
                    "ANALYZER: Calculated loads, sending to output");

    queue_enqueue(analyzer->output, loads, 1,
                  sizeof(loads->loads[0]) * numberOfLines);

    free(prevLines);
    free(nextLines);
    cpuLoads_destroy(loads);
    free(samples);
  }
  analyzer_cleanUp(analyzer, 0);
}

int analyzer_createThread(analyzer *analyzer, thrd_t *thread) {
  if (analyzer && analyzer->output) {
    return thrd_create(thread, analyzer_runAnalyzer, analyzer);
  } else {
    return thrd_error;
  }
}
