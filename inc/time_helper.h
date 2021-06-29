#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <time.h>

#define SEC_TO_NS(s) ((s)*1000000000)
#define NS_DIV_SEC(ns) (ns / 1000000000)
#define NS_MOD_SEC(ns) (ns % 1000000000)

static inline long time_helper_nanosecondsTimeStamp() {
  struct timespec t;
  timespec_get(&t, TIME_UTC);
  return SEC_TO_NS(t.tv_sec) + t.tv_nsec;
}

#endif
