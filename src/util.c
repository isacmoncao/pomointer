#include <stdlib.h>
#include <string.h>
#include "../include/util.h"

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
