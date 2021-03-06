#include <program_state.h>

program_state *program_state_create(bool initValue) {
  program_state *toReturn = malloc(sizeof(program_state));
  if (toReturn) {
    atomic_store(&toReturn->isRunning, initValue);
  }
  return toReturn;
}

void program_state_destroy(program_state *);
