#include <common.h>
#include <stdlib.h>

cpuLoads *cpuLoads_create(size_t size) {
  return malloc(sizeof(cpuLoads) + size * sizeof(double));
}
void cpuLoads_destroy(cpuLoads *loads) { free(loads); }
