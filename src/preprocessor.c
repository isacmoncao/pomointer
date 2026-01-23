#include <string.h>
#include "preprocessor.h"
#include "util.h"

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

  char line[1024];
  char include_path[256];

  while (fgets(line, sizeof(line), file) != NULL) {
    char* trimmed = trim_left(line);

    // Checks if it's an include
    if (strncmp(trimmed, "#include", 8) == 0) {
      char* quote_start = strchr(trimmed, '"');

      if (quote_start) {
        char* quote_end = strchr(quote_start + 1, '"');

        if (quote_end) {
          size_t len = quote_end - (quote_start + 1);
          if (len >= sizeof(include_path)) {
            fprintf(stderr, "Error: include path too long\n");
            fclose(file);
            fclose(output);
            return NULL;
          }

          strncpy(include_path, quote_start + 1, len);
          include_path[len] = '\0';

          FILE* included = preprocess_file(include_path, depth + 1);
          if (included != NULL) {
            char buffer[1024];
            rewind(included);

            while (fgets(buffer, sizeof(buffer), included) != NULL) {
              fputs(buffer, output);
            }

            fclose(included);
          }
          continue;
        }
      }
    }
    // Normal line 
    fputs(line, output);
  }

  fclose(file);
  rewind(output);
  return output;
}


