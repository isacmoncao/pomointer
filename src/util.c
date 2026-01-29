#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <sys/stat.h>
#include "util.h"

char** split_string(const char* str, const char delimiter, int* count) {
  // Check for valid input
  if (str == NULL) {
    *count = 0;
    return NULL;
  }

  int len = strlen(str);
  if (len == 0) {
    *count = 0;
    // Returns an empty array
    char** result = malloc(sizeof(char*));
    result[0] = NULL;
    return result;
  }

  // Counting how many parts we'll have 
  int parts = 1;
  for (int i = 0; i < len; i++) {
    if (str[i] == delimiter) {
      parts++;
    }
  }

  // Allocate an array of pointers to strings
  char** result = malloc((parts + 1) * sizeof(char*)); // +1 for NULL at the end
  if (result == NULL) {
    *count = 0;
    return NULL;
  }

  int current_part = 0;
  int start = 0;

  for (int i = 0; i <= len; i++) {
    if (i == len || str[i] == delimiter) {
      // Calculate size of substring
      int substr_len = i - start;

      // Allocate memory for substring
      result[current_part] = malloc((substr_len + 1) * sizeof(char));
      if (result[current_part] == NULL) {
        // Free memory in case of error
        for (int j = 0; j < current_part; j++) {
          free(result[j]);
        }

        free(result);
        *count = 0;
        return NULL;
      }

      // Copies substring
      if (substr_len > 0) {
        strncpy(result[current_part], &str[start], substr_len);
      }
      result[current_part][substr_len] = '\0';

      // Proceed to the next part
      current_part++;
      start = i + 1;
    }
  }

  // Append NULL at the end of the array
  result[current_part] = NULL;
  *count = current_part;

  return result;
}

// Frees an array of strings. The last element should be NULL.
void free_string_array(char** array) {
  if (array == NULL) return;

  for (int i = 0; array[i] != NULL; i++) {
    free(array[i]);
  }

  free(array);
}

// Count '*' in a string
int count_stars(const char* str) {
  if (str == NULL) {
    return 0;
  }

  int count = 0;
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    if (str[i] == '*') {
      count++;
    }
  }

  return count;
}

// Removes the specified 'chars' from the beginning and end of
// the string. The default is to remove white spaces.
char* strip_string(const char* str, const char* chars) {
  if (str == NULL) return NULL;

  bool use_default = (chars == NULL || *chars == '\0');
  const char* start = str;
  const char* end = str + strlen(str) - 1;

  // Find the beginning
  while (*start) {
    bool remove = false;
    if (use_default) {
      remove = (*start == ' ' || *start == '\t' || *start == '\n' ||
                *start == '\r' || *start == '\f' || *start == '\v');
    } else {
      for (const char* p = chars; *p; p++) {
        if (*p == *start) {
          remove = true;
          break;
        }
      }
    }

    if (!remove) break;
    start++;
  }

  // Find the end
  while (end > start) {
    bool remove = false;
    if (use_default) {
      remove = (*end == ' ' || *end == '\t' || *end == '\n' ||
                *end == '\r' || *end == '\f' || *end == '\v');
    } else {
      for (const char* p = chars; *p; p++) {
        if (*p == *end) {
          remove = true;
          break;
        }
      }
    }

    if (!remove) break;
    end--;
  }

  // Allocate new string
  size_t len = (end >= start) ? (end - start + 1) : 0;
  char* result = malloc(len+1);
  if (result == NULL) return NULL;

  if (len > 0) {
    memcpy(result, start, len);
  }
  result[len] = '\0';

  return result;
}

// Trim whitespaces from the left
char* trim_left(char* str) {
  while (isspace((unsigned char) *str)) {
    str++;
  }

  return str;
}

char* int_to_string(int n) {
  int length = snprintf(NULL, 0, "%d", n);

  char* str = malloc(length + 1);
  if (str == NULL) {
    return NULL;
  }

  snprintf(str, length+1, "%d", n);

  return str;
}

int string_to_int(const char* str) {
  char* endptr;
  errno = 0;

  long val = strtol(str, &endptr, 10);

  if (errno == ERANGE) {
    fprintf(stderr, "Error: number out of range\n");
    return 0;
  }

  if (endptr == str) {
    fprintf(stderr, "Error: no digit found at string '%s'\n", str);
    return 0;
  }

  if (*endptr != '\0') {
    fprintf(stderr, "Warning: extra characters -> %s\n", endptr);
  }

  if (val > INT_MAX || val < INT_MIN) {
    fprintf(stderr, "Error: value out int range\n");
    return 0;
  }

  return (int)val;
}

bool is_empty_str(const char *str) {
  while (*str) {
    if (!isspace((unsigned char) *str)) {
      return false;
    }
    str++;
  }
  return true;
}

bool is_comment(const char *str) {
  while (isspace((unsigned char)*str)) {
    str++;
  }

  return (*str == '#');
}

bool string_arr_contains(char** array, const char* string) {
  if (!array || !string) return false;

  bool contains = false;
  for (int i = 0; array[i] != NULL; i++) {
    if (strcmp(array[i], string) == 0) {
      contains = true;
      break;
    }
  }

  return contains;
}

static bool is_leap_year(int y) {
  bool div4 = y % 4 == 0;
  bool div400 = y % 400 == 0;
  bool div100 = y % 100 == 0;
  
  if (div4 && (div400 || !div100))
    return true;

  return false;
}

static bool is_valid_day(int d) {
  return (d >= 1 && d <= 31);
}

static bool is_valid_month(int m) {
  return (m >= 1 && m <= 12);
}

static bool is_valid_year(int y) {
  return (y >= 0 && y <= 9999);
}

static bool is_valid_date(int day, int month, int year) {
  // dd/mm/yyyy
  if (!is_valid_day(day) || !is_valid_month(month) || !is_valid_year(year)) {
    return false;
  }

  // Day 31 in months of 30 days
  if (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11)) {
    return false;
  }

  // February
  if (month == 2) {
    if (is_leap_year(year) && day > 29) {
      return false;
    } else if (!is_leap_year(year) && day > 28) {
      return false;
    }
  }

  return true;
}

time_t string_to_time(const char *str) {
  struct tm time = {0};
  int d, m, y;

  if (sscanf(str, "%d/%d/%d", &d, &m, &y) != 3) {
    return (time_t)-1; 
  }

  if (!is_valid_day(d) || !is_valid_month(m) || !is_valid_year(y)) {
    return (time_t)-1;
  } 

  if (!is_valid_date(d, m, y)) {
    return (time_t)-1;
  }

  if (y >= 0 && y < 100 ) {
     // Adjust 0-69 are related to 2000, and 70-99 are related to 1900
     y += (y < 70) ? 2000 : 1900;
  }

  time.tm_mday = d;
  time.tm_mon = m - 1; // Adjust to 0-11  
  time.tm_year = y - 1900; // Years since 1900
  time.tm_isdst = -1;

  time_t result = mktime(&time);

  if (result == (time_t)-1) {
    return (time_t)-1;
  }

  return result;
}

char* time_to_string(time_t time) {
  struct tm* t = localtime(&time);
  char* buffer = malloc(80 * sizeof(char));
  if (buffer == NULL) {
    return NULL;
  }
  strftime(buffer, 80, "%d/%m/%Y", t);
  return buffer;
}

time_t get_file_mod_date(const char* path) {
  struct stat file_info;

  // Check if it exists
  if (stat(path, &file_info) == -1) {
    fprintf(stderr, "Error: cannot acces '%s'\n", path);
    return -1;
  }

  time_t modtime = file_info.st_mtime;

  return modtime;
}

void print_file(const char* path) {
  FILE* f = fopen(path, "r");
  if (f == NULL) {
    fprintf(stderr, "Error: cannot read '%s'\n", path);
    return ;
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), f) != NULL) {
    buffer[strcspn(buffer, "\n")] = '\0';

    printf("%s\n", buffer);
  }

  fclose(f);
}

// This function extracts the directory from a path
void extract_directory(const char *path, char *dir_buff, size_t buffer_size) {
  if (path == NULL || dir_buff == NULL) {
    if (buffer_size > 0) {
      dir_buff[0] = '\0';
      return ;
    }
  } 

  strncpy(dir_buff, path, buffer_size - 1);
  dir_buff[buffer_size - 1] = '\0';

  // Find last '/' or '\' if on Windows
  char* last_slash = strrchr(dir_buff, '/');

#ifdef _WIN32
  char* last_backslash = strrch(dir_buff, '\\');
  if (last_backslash != NULL && (last_slash == NULL || last_backslash > last_slash)) {
    last_slash = last_backslash;
  } 
#endif

  if (last_slash != NULL) {
    *last_slash = '\0'; // Ends at directory
  } else {
    // No '/' because it's at the current directory
    strncpy(dir_buff, ".", buffer_size);
  }
}
