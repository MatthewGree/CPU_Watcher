#include <common.h>
#include <reader.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <string.h>
#include <time.h>

#define READER_LINE_SIZE 100
#define READER_SAMPLING_WAIT_NANOSECONDS 500000000
#define READER_SAMPLING_WAIT_SECONDS 0
#define READER_NUM_OF_SAMPLES 2
#define READER_LOG_MSG_SIZE 64
#define READER_STAT_FILE "/proc/stat"

struct reader {
  queue *output;
  logger *logger;
  program_state *state;
};

reader *reader_create(logger *logger, program_state *state) {
  if (!logger) {
    return 0;
  }
  reader *toReturn = malloc(sizeof(reader));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->state = state;
    toReturn->output = 0;
  }
  return toReturn;
}

void reader_destroy(reader *);

bool reader_setOutput(reader *reader, queue *output) {
  if (reader->output || !output) {
    return false;
  } else {
    reader->output = output;
    return true;
  }
}

static char *reader_getData(FILE *file, unsigned short int lines,
                            char *toReturn) {
  file = fopen(READER_STAT_FILE, "r");
  char line[READER_LINE_SIZE];

  if (!file) {
    return 0;
  }

  toReturn[0] = 0;
  for (unsigned short int i = 0; i < lines; i++) {
    if (fgets(line, READER_LINE_SIZE, file)) {
      strcat(toReturn, line);
    }
  }
  fclose(file);

  return toReturn;
}

static unsigned short int calculateNumOfProperLines(FILE *file) {
  unsigned short int properLines = 0;
  char line[READER_LINE_SIZE];
  while (true) {
    if (fgets(line, READER_LINE_SIZE, file)) {
      if (strstr(line, "cpu")) {
        properLines++;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  return properLines;
}

noreturn static void reader_cleanUp(reader *reader, int exitCode) {
  queue_enqueue(reader->output, 0, 0, 0);
  thrd_exit(exitCode);
}

static int reader_runReader(void *reader_void) {
  reader *reader = reader_void;
  FILE *cpuStats = fopen(READER_STAT_FILE, "r");
  if (!cpuStats) {
    logger_printLog(reader->logger,
                    "READER: Unable to open /proc/stat, exiting...");
    reader_cleanUp(reader, 1);
  }
  unsigned short int properLines = calculateNumOfProperLines(cpuStats);
  fclose(cpuStats);
  if (!properLines) {
    logger_printLog(reader->logger,
                    "READER: File not valid, at least 2 lines expected");
    reader_cleanUp(reader, 1);
  }
  char logMessage[READER_LOG_MSG_SIZE];
  sprintf(logMessage, "READER: initialized, found %d cores", properLines - 1);
  logger_printLog(reader->logger, logMessage);

  char *stats = malloc(sizeof(char) * properLines * READER_LINE_SIZE);
  char *allSamples = malloc(sizeof(char) * properLines * READER_LINE_SIZE *
                            READER_NUM_OF_SAMPLES);

  if (!stats || !allSamples) {
    logger_printLog(reader->logger,
                    "READER: not enough storage on heap, exiting");
    reader_cleanUp(reader, 1);
  }

  logger_printLog(reader->logger, "READER: entering main loop");
  while (atomic_load(&reader->state->isRunning)) {
    allSamples[0] = 0;
    for (unsigned short int i = 0; i < READER_NUM_OF_SAMPLES; i++) {
      if (!reader_getData(cpuStats, properLines, stats)) {
        logger_printLog(reader->logger,
                        "READER: Unable to open /proc/stat, exiting...");
        reader_cleanUp(reader, 1);
      }
      strcat(allSamples, stats);
      if (i < READER_NUM_OF_SAMPLES - 1) {
        strcat(allSamples, COMMON_STATS_DELIMITER);
        thrd_sleep(
            &(struct timespec){.tv_sec = READER_SAMPLING_WAIT_SECONDS,
                               .tv_nsec = READER_SAMPLING_WAIT_NANOSECONDS},
            0);
      }
    }
    queue_enqueue(reader->output, allSamples, strlen(allSamples) + 1, 0);
    sprintf(logMessage, "READER: finished sending a set of information");
    logger_printLog(reader->logger, logMessage);
  }
  free(stats);
  free(allSamples);
  reader_cleanUp(reader, 0);
}

int reader_createThread(reader *reader, thrd_t *thread) {
  if (reader && reader->output) {
    return thrd_create(thread, reader_runReader, reader);
  } else {
    return thrd_error;
  }
}
