#include <stdio.h>
#include <stdlib.h>
#include "pomofile.h"

static PomoFile* pomofiles;
static HashMap* global_registers;
static HashMap* global_pomodoros;

static void clear_hashmaps(void);

static void clear_hashmaps(void) {
  hashmap_destroy(global_registers, NULL);
  hashmap_destroy(global_pomodoros, NULL);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file0> <file1> ... <fileN>\n", argv[0]); 
    return EXIT_FAILURE;
  }

  int num_files = argc - 1;
  pomofiles = (PomoFile*)malloc((argc - 1) * sizeof(PomoFile));
  global_registers = hashmap_create(16, 0.75);
  global_pomodoros = hashmap_create(16, 0.75);

  if (pomofiles == NULL) {
    fprintf(stderr, "Error: cannot allocate memory\n");
    return EXIT_FAILURE;
  }

  for (int i = 1, j = 0; i < argc; i++, j++) {
    if (pomofile_init(&pomofiles[j], argv[i]) != 0) {
      fprintf(stderr, "Error: cannot initialize PomoFile for '%s'\n", argv[i]);

      for (int k = 0; k < j; k++) {
        free_pomofile(&pomofiles[k]);
      }
      free(pomofiles);
      clear_hashmaps();
      return EXIT_FAILURE;
    }
  }
  
  for (int i = 0; i < num_files; i++) {
    parse_file(&pomofiles[i], global_registers, global_pomodoros);
  }

  hashmap_foreach(global_registers, process_global_registers, (void*)global_pomodoros);

  for (int i = 0; i < num_files; i++) {
    free_pomofile(&pomofiles[i]);
  }
  free(pomofiles);
  clear_hashmaps();
  return EXIT_SUCCESS;
}

