#ifndef POMOINTER_UTIL_H
#define POMOINTER_UTIL_H

#include <stdbool.h>
#include <time.h>

// Strings
int count_stars(const char* str);
char** split_string(const char* str, const char delimiter, int* count);
char* strip_string(const char* str, const char* chars);
void free_string_array(char** array);

// Conversions
int string_to_int(const char* str);
char* int_to_string(int n);
time_t string_to_time(const char* str);
char* time_to_string(time_t time);

// Booleans
bool is_empty_str(const char* str);
bool is_comment(const char* str);

// Files
time_t get_file_mod_date(const char* path);
void print_file(const char* path);

#endif
