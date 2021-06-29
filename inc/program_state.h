#ifndef PROGRAM_STATE_H
#define PROGRAM_STATE_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct program_state program_state;
/*
 * used to hold variables defining program state
 */
struct program_state {
  atomic_bool isRunning;
};

program_state *program_state_create(bool initValue);
inline void program_state_destroy(program_state *ptr) {
  if (ptr) {
    free(ptr);
  }
}

#endif
