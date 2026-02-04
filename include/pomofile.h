/*
 * pomointer - .pf file interpreter
 * Copyright (c) 2026 José Isac Araujo Monção
 * 
 * See LICENSE file for full BSD 3-Clause license terms.
 */
#ifndef POMOINTER_POMOFILE_H
#define POMOINTER_POMOFILE_H

#include <time.h>
#include <stdbool.h>
#include "hashmap.h"
#include "process_data.h"

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
  int pomodoro_duration; // Minutes
} PomoFile;

int pomofile_init(PomoFile* pomofile, const char* path);
int parse_file(PomoFile* pomofile, HashMap* global_registers, ProcessData* process_data);
void free_pomofile(PomoFile* pomofile);
void print_pomofile(PomoFile* pomofile);
void process_final_registers(const char* date, void* registers, void* process_data);
void filter_registers(ProcessData* process_data, HashMap* filtered_registers);

#endif
