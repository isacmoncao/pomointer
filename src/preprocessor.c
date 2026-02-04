/*
 * pomointer - .pf file interpreter
 * Copyright (c) 2026 José Isac Araujo Monção
 * 
 * See LICENSE file for full BSD 3-Clause license terms.
 */
#include <stdio.h>
#include <string.h>
#include "preprocessor.h"
#include "util.h"

// Recursive function that preprocess a file
// Only directive supported: #include
FILE* preprocess_file(const char* path, int depth) {
  if (depth >= MAX_INCLUDE_DEPTH) {
    fprintf(stderr, "Error: max depth of includes reached\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: cannot read file '%s'\n", path);
    return NULL;
  }

  FILE* output = tmpfile();
  if (output == NULL) {
    fclose(file);
    return NULL;
  }

  char current_dir[1024];
  extract_directory(path, current_dir, sizeof(current_dir));

  char line[2048];
  char full_include_path[2048];

  while (fgets(line, sizeof(line), file) != NULL) {
    char* trimmed = trim_left(line);
    
    if (strncmp(trimmed, "#include", 8) == 0) {
      char* quote_start = strchr(trimmed, '"');

      if (quote_start) {
        // quote_start + 1 because quote_start is '"'
        char* quote_end = strchr(quote_start + 1, '"');

        if (quote_end) {
          size_t len = quote_end - (quote_start + 1);
          char include_filename[256];
          // Copies exactly the name of the include file
          strncpy(include_filename, quote_start + 1, len);
          include_filename[len] = '\0';

          // Determines full include path
          if (include_filename[0] == '/'
#ifdef _WIN32
              || include_filename[0] == '\\'
              || (isalpha(include_filename[0]) && include_filename[1] == ':')
#endif
          ) {
            strncpy(full_include_path, include_filename, sizeof(full_include_path) - 1);
          } else {
            // Relative path
            snprintf(full_include_path, sizeof(full_include_path), "%s/%s", current_dir, include_filename);
          }

          FILE* processed_include = preprocess_file(full_include_path, depth + 1);

          if (processed_include != NULL) {
            // Copies processed content
            char buffer[1024];
            rewind(processed_include);

            while (fgets(buffer, sizeof(buffer), processed_include) != NULL) {
              fputs(buffer, output);
            }

            fclose(processed_include);
          } else {
            fprintf(stderr, "Warning: could not preprocess file '%s' included from '%s'\n", include_filename, path);
            fputs(line, output);
          }
          continue;
        }
      }
    }

    fputs(line, output);
  }

  fclose(file);
  rewind(output);
  return output;
}
