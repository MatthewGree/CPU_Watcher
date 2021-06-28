#include <reader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define READER_LINE_SIZE 256
#define READER_STATS_SIZE 2048
#define READER_SAMPLING_WAIT_NANOSECONDS 500000000
#define READER_SAMPLING_WAIT_SECONDS 0
#define READER_NUM_OF_SAMPLES 2
#define READER_LOG_MSG_SIZE 64
#define READER_STAT_FILE "/proc/stat"

struct reader {
  queue_string *output;
  logger *logger;
  program_state *state;
};

reader *reader_create(logger *logger, program_state *state) {
  reader *toReturn = malloc(sizeof(reader));
  if (toReturn) {
    toReturn->logger = logger;
    toReturn->state = state;
    toReturn->output = 0;
  }
  return toReturn;
}

void reader_destroy(reader *reader) { free(reader); }

bool reader_setOutput(reader *reader, queue_string *output) {
  if (reader->output) {
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

static int reader_runReader(void *reader_void) {
  reader *reader = reader_void;
  FILE *cpuStats = fopen(READER_STAT_FILE, "r");
  if (!cpuStats) {
    logger_printLog(reader->logger,
                    "READER: Unable to open /proc/stat, exiting...");
    thrd_exit(1);
  }
  unsigned short int properLines = calculateNumOfProperLines(cpuStats);
  fclose(cpuStats);
  if (!properLines) {
    logger_printLog(reader->logger,
                    "READER: File not valid, at least 2 lines expected");
    thrd_exit(1);
  }
  char logMessage[READER_LOG_MSG_SIZE];
  sprintf(logMessage, "Reader initialized, found %d cores", properLines - 1);
  logger_printLog(reader->logger, logMessage);

  char stats[READER_STATS_SIZE] = "";

  while (atomic_load(&reader->state->isRunning)) {
    logger_printLog(reader->logger, "entering Reader loop");
    for (int i = 0; i < READER_NUM_OF_SAMPLES; i++) {
      if (!reader_getData(cpuStats, properLines, stats)) {
        logger_printLog(reader->logger,
                        "READER: Unable to open /proc/stat, exiting...");
        thrd_exit(1);
      }
      queue_string_enqueue(reader->output, stats);
      if (i < READER_NUM_OF_SAMPLES - 1) {
        thrd_sleep(
            &(struct timespec){.tv_sec = READER_SAMPLING_WAIT_SECONDS,
                               .tv_nsec = READER_SAMPLING_WAIT_NANOSECONDS},
            &(struct timespec){.tv_sec = 0, .tv_nsec = 0});
      }
    }
    sprintf(logMessage, "Reader finished sending a set of information");
    logger_printLog(reader->logger, logMessage);
  }

  thrd_exit(0);
}

int reader_createThread(reader *reader, thrd_t *thread) {
  if (reader->output) {
    return thrd_create(thread, reader_runReader, reader);
  } else {
    return thrd_error;
  }
}