#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>

#define COMMON_STATS_DELIMITER "|"

typedef struct cpuLoads cpuLoads;

struct cpuLoads {
  size_t size;
  double loads[];
};

cpuLoads *cpuLoads_create(size_t size);
void cpuLoads_destroy(cpuLoads *loads);

#endif
