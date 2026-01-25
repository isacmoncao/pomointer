#include <stdio.h>
#include <stdlib.h>
#include "pomofile.h"

static PomoFile* pomofiles_array;
static HashMap* global_registers;
static HashMap* global_pomodoro_durations;

static void clear_hashmaps(void);

static void clear_hashmaps(void) {
  hashmap_destroy(global_registers, NULL);
  hashmap_destroy(global_pomodoro_durations, NULL);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file0> <file1> ... <fileN>\n", argv[0]); 
    return EXIT_FAILURE;
  }

  int num_files = argc - 1;
  pomofiles_array = (PomoFile*)malloc((num_files) * sizeof(PomoFile));
  global_registers = hashmap_create(16, 0.75);
  global_pomodoro_durations = hashmap_create(16, 0.75);

  if (pomofiles_array == NULL) {
    fprintf(stderr, "Error: cannot allocate memory\n");
    return EXIT_FAILURE;
  }

  for (int i = 1, j = 0; i < argc; i++, j++) {
    if (pomofile_init(&pomofiles_array[j], argv[i]) != 0) {
      fprintf(stderr, "Error: cannot initialize PomoFile for '%s'\n", argv[i]);

      for (int k = 0; k < j; k++) {
        free_pomofile(&pomofiles_array[k]);
      }
      free(pomofiles_array);
      clear_hashmaps();
      return EXIT_FAILURE;
    }
  }
  
  for (int i = 0; i < num_files; i++) {
    parse_file(&pomofiles_array[i], global_registers, global_pomodoro_durations);
  }

  hashmap_foreach(global_registers, process_global_registers, global_pomodoro_durations);

  for (int i = 0; i < num_files; i++) {
    free_pomofile(&pomofiles_array[i]);
  }
  free(pomofiles_array);
  clear_hashmaps();
  return EXIT_SUCCESS;
}

