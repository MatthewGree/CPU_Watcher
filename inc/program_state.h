#ifndef PROGRAM_STATE_H
#define PROGRAM_STATE_H

#include <stdatomic.h>
#include <stdbool.h>

typedef struct program_state program_state;

struct program_state {
  atomic_bool isRunning;
};

program_state *program_state_create(bool initValue);
void program_state_destroy(program_state *ptr);

#endif
