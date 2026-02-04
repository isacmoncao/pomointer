/*
 * pomointer - .pf file interpreter
 * Copyright (c) 2026 José Isac Araujo Monção
 * 
 * See LICENSE file for full BSD 3-Clause license terms.
 */
#ifndef POMOINTER_UTIL_H
#define POMOINTER_UTIL_H

#include <stdbool.h>
#include <time.h>

// Strings
int count_stars(const char* str);
char** split_string(const char* str, const char delimiter, int* count);
char* strip_string(const char* str, const char* chars);
void free_string_array(char** array);
char* trim_left(char* str);

// Conversions
int string_to_int(const char* str);
char* int_to_string(int n);
time_t string_to_time(const char* str);
char* time_to_string(time_t time);

// Booleans
bool is_empty_str(const char* str);
bool is_comment(const char* str);
bool string_arr_contains(char** array, const char* string);

// Files
time_t get_file_mod_date(const char* path);
void print_file(const char* path);
void extract_directory(const char* path, char* dir_buff, size_t buffer_size);

#endif
