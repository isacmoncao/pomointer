#ifndef POMOINTER_PROCESS_DATA_H
#define POMOINTER_PROCESS_DATA_H

#include <stdbool.h>
#include <time.h>
#include "hashmap.h"

typedef struct {
  bool aftdate_flag;
  bool befdate_flag;
  time_t after_date;
  time_t before_date;
} RegisterFilter;

typedef struct {
  HashMap* pomodoro_durations;
  RegisterFilter register_filter;
} ProcessData;

#endif 
