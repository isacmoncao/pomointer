#ifndef POMOINTER_UTIL_H
#define POMOINTER_UTIL_H

#include <stdbool.h>

char** split_string(const char* str, const char delimiter, int* count);
void free_string_array(char** array);
int count_stars(const char* str);
char* strip_string(const char* str, const char* chars);
char* int_to_string(int n);
int string_to_int(const char* str);
bool is_empty_str(const char* str);
bool is_comment(const char* str);


#endif
