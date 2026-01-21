#ifndef POMOINTER_UTIL_H
#define POMOINTER_UTIL_H

char** split_string(const char* str, const char delimiter, int* count);
void free_string_array(char** array);
int count_stars(const char* str);

#endif
