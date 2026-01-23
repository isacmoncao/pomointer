#ifndef POMOINTER_POMOFILE_H
#define POMOINTER_POMOFILE_H

#include <time.h>
#include <stdbool.h>
#include "hashmap.h"

typedef enum {
  LINE_ASSIGNMENT,
  LINE_REGISTER,
  LINE_INVALID
} LineType;

typedef struct {
  const char* path;
  HashMap* assignments;
  HashMap* registers;
  time_t date;
  int pomodoro_size; // Minutes
} PomoFile;

int pomofile_init(PomoFile* file, const char* path);
int parse_file(PomoFile* pomofile, HashMap* reg, HashMap* pomos);
void free_pomofile(PomoFile* pomofile);
void print_pomofile(PomoFile* pomofile);
void process_global_registers(const char* key, void* value, void* glb_pomodoros);
#endif
