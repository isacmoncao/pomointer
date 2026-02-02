#ifndef POMOINTER_PROCESS_DATA_H
#define POMOINTER_PROCESS_DATA_H

#include <stdbool.h>
#include <time.h>
#include "hashmap.h"

typedef struct {
  bool aftdate_flag;
  bool befdate_flag;
  bool subj_flag;
  bool export_flag;
  time_t after_date;
  time_t before_date;
  char** subjects;
  char* export_type;
} RegisterFilter;

typedef struct {
  HashMap* pomodoro_durations;
  HashMap* global_registers;
  RegisterFilter register_filter;
} ProcessData;

#endif 
