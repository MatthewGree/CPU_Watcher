#include <program_state.h>
#include <stdlib.h>

program_state *program_state_create(bool initValue) {
  program_state *toReturn = malloc(sizeof(program_state));
  if (toReturn) {
    toReturn->isRunning = initValue;
  }
  return toReturn;
}

void program_state_destroy(program_state *ptr) { free(ptr); }
