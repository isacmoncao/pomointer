#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "pomofile.h"
#include "preprocessor.h"

// AUXILIARY FUNCTIONS PROTOTYPES
static void print(const char* key, void* value, void* type);
static int merge_pomodoros(HashMap* dest, HashMap* src);
static int search_abbvr(HashMap* assignments, HashMap* registers);
static void process_register(const char* key, void* value, void* pomodoro_duration);

static int read_assignment(char* line, HashMap* assignments);
static int read_register(char* line, HashMap* registers);
static int get_pomodoro_duration(HashMap* assignments);
static time_t get_date(HashMap* assignments);
static LineType classify_line(char* line);

static bool ispomodefined(HashMap* assignments);
static bool isdatedefined(HashMap* assignments);

// ------------------ AUXILIARY FUNCTIONS -------------------------------------------
static void print(const char* key, void* value, void* type) {
  if (strcmp("ASSIGNMENT", (const char*)type) == 0) {
    printf("\"%s\":\"%s\",\n", key, (char*)value);
  } else {
    int pomodoros = string_to_int((char*)value);
    printf("\"%s\":\"%d\",\n", key, pomodoros);
  }
}

static int merge_pomodoros(HashMap* dest, HashMap* src) {
  if (!dest || !src ) return 0;
  int added_elements = 0;
  for (int i = 0; i < src->capacity; i++) {
    Entry* entry = src->buckets[i];
    while (entry) {
      int current_value = string_to_int(hashmap_get(dest, entry->key));

      if (current_value) {
        int new_value = current_value + string_to_int(entry->value);
        hashmap_put(dest, entry->key, int_to_string(new_value));
      } else {
        hashmap_put(dest, entry->key, entry->value);
        added_elements++;
      }
      entry = entry->next;
    }
  }
  return added_elements;
}

static int search_abbvr(HashMap* assignments, HashMap* registers) {
  if (!assignments || !registers) return 0;
  int modified = 0;
  for (int i = 0; i < registers->capacity; i++) {
    Entry* entry = registers->buckets[i];
    while (entry) {
      void* found_assignment = hashmap_get(assignments, entry->key);

      if (found_assignment) {
        entry->key = (char*)found_assignment;
        modified++;
      }

      entry = entry->next;
    }
  }

  return modified;
}

static void process_register(const char* key, void* value, void* pomodoro_duration) {
  // Key is the date, value is the ammount of pomodoros
  printf("%s:\n", key);
  int pomodoros = string_to_int((const char*)value);
  int* duration = (int*)pomodoro_duration;
  for (int i = 0; i < pomodoros; i++) {
    printf("🍅");
  }
  printf(" -> %d minutes\n", pomodoros * *duration);
}

static LineType classify_line(char *line) {
  char* start = line; 
  // Skips initial spaces
  while(isspace(*start)) start++; 
  
  char* has_equal = strchr(line, '=');
  char* has_colon = strchr(line, ':');

  if (has_equal && !has_colon) {
    return LINE_ASSIGNMENT;
  }

  if (!has_equal && has_colon) {
    return LINE_REGISTER;
  } 

  return LINE_INVALID;
}

static int read_assignment(char* line, HashMap* assignments) {
  // Reads an assignment at the line and put it in a hashmap
  int parts_count;
  char** parts = split_string(line, '=', &parts_count);

  if (parts && parts_count == 2) {
    char* key = strip_string(parts[0], NULL);
    char* val = strip_string(parts[1], NULL);

    hashmap_put(assignments, key, (void*)val);
    free_string_array(parts);

    return 1;
  }

  free_string_array(parts);
  return 0;
}

static int read_register(char *line, HashMap *registers) {
  // Reads a register at the line and put it in a hashmap
  int parts_count;
  char** parts = split_string(line, ':', &parts_count);
  
  if (parts && parts_count == 2) {
    char* key = strip_string(parts[0], NULL);
    char* val = strip_string(parts[1], NULL);

    // If it exists
    if (hashmap_get(registers, key) != NULL) {
      // Update it
      int old = string_to_int((char*)hashmap_get(registers, key));
      int current = count_stars(val);
      int new = old + current;
      hashmap_put(registers, key, int_to_string(new));
    } else {
      hashmap_put(registers,
                  key,
                  int_to_string(count_stars(val))
                  );
    }

    free_string_array(parts);
    return 1;
  }

  free_string_array(parts);
  return 0;
}

static bool ispomodefined(HashMap* assignments) {
  bool exists = hashmap_contains(assignments, "POMO");

  if (!exists) {
    return false;
  }

  char* val = (char*)hashmap_get(assignments, "POMO");
  int minutes = string_to_int(val);

  if (minutes != 0) {
    return true;
  }
  
  return false;
}

static int get_pomodoro_duration(HashMap *assignments) {
  char* val = (char*)hashmap_get(assignments, "POMO");
  int minutes = string_to_int(val);

  return minutes;
}

static bool isdatedefined(HashMap* assignments) {
  bool exists = hashmap_contains(assignments, "DATE");

  if (!exists) {
    return false;
  }

  char* val = (char*)hashmap_get(assignments, "DATE");
  if (string_to_time(val) == (time_t)-1) {
    return false;
  }

  return true;
}

static time_t get_date(HashMap* assignments) {
  char* val = (char*)hashmap_get(assignments, "DATE");
  return string_to_time(val);
}

// -----------------------------------------------------------------------

int pomofile_init(PomoFile* file, const char* path) {
  file->path = path;
  file->assignments = hashmap_create(16, 0.75);
  file->registers = hashmap_create(16, 0.75);

  if (!file->assignments || !file->registers) {
    free_pomofile(file);
    return -1;
  }

  file->date = get_file_mod_date(path);
  file->pomodoro_duration = 30;
  return 0;
}

void print_pomofile(PomoFile* pomofile) {
  if (!pomofile) return ;

  printf("POMOFILE: %s\n", pomofile->path);
  printf("----------------------------------------------\n");
  printf("Assignments: ");
  if (pomofile->assignments != NULL) {
    printf("{\n   ");
    hashmap_foreach(pomofile->assignments, print, "ASSIGNMENT");
    printf("\n}\n");
  } else {
    printf("NONE\n");
  }
  printf("Registers: ");
  if (pomofile->registers != NULL) {
    printf("{\n   ");
    hashmap_foreach(pomofile->registers, print, "REGISTER");
    printf("\n}\n");
  } else {
    printf("NONE\n");
  }
  printf("Date: %s\n", time_to_string(pomofile->date));
  printf("Pomodoro duration: %d\n", pomofile->pomodoro_duration);
  printf("----------------------------------------------\n");
}


void free_pomofile(PomoFile* pomofile) {
  if (!pomofile) return;

  if (pomofile->assignments) {
    hashmap_destroy(pomofile->assignments, NULL);
  }
  if (pomofile->registers) {
    hashmap_destroy(pomofile->registers, NULL);
  }

  pomofile->path = NULL;
  pomofile->date = -1;
  pomofile->pomodoro_duration = 0;
  pomofile->assignments = NULL;
  pomofile->registers = NULL;
}

int parse_file(PomoFile* pomofile, HashMap* reg, HashMap* pomos) {
  FILE* f = preprocess_file(pomofile->path, 0);
  if (f == NULL) {
    fprintf(stderr, "Erro: cannot read file '%s'\n", pomofile->path);
    return -1;
  }

  char line[1024];
  int line_n = 0;
  while (fgets(line, sizeof(line), f) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    line_n++;

    if (is_empty_str(line)) {
      continue;
    }
    if (is_comment(line)) {
      continue;
    }
    
    LineType t = classify_line(line);

    if (t == LINE_ASSIGNMENT) {
      read_assignment(line, pomofile->assignments);
    }
    if (t == LINE_REGISTER) {
      read_register(line, pomofile->registers);
    }
    if (t == LINE_INVALID) {
      fprintf(stderr, "Error: invalid line at %s:%d\n", pomofile->path, line_n);
      fclose(f); 
      return -1;
    }
  }

  if (ispomodefined(pomofile->assignments)) {
    pomofile->pomodoro_duration = get_pomodoro_duration(pomofile->assignments);
  }
  if (isdatedefined(pomofile->assignments)) {
    pomofile->date = get_date(pomofile->assignments);
  }

  search_abbvr(pomofile->assignments, pomofile->registers);

  // If there's no entry in global registers, create new
  if (hashmap_get(reg, time_to_string(pomofile->date)) == NULL) {
    hashmap_put(reg, time_to_string(pomofile->date), (void*)pomofile->registers);
  } else {
    // If it already exists, update it
    HashMap* old = hashmap_get(reg, time_to_string(pomofile->date)); 
    HashMap* new = pomofile->registers;

    merge_pomodoros(old, new);
  }

  // Pomodoro duration for that day
  hashmap_put(pomos, time_to_string(pomofile->date), int_to_string(pomofile->pomodoro_duration));

  fclose(f); 
  return 1;
}

void process_global_registers(const char* key, void* value, void* glb_pomodoros) {
  // Key is the date, value is a hashmap of registers
  HashMap* g_pomodoros = (HashMap*)glb_pomodoros;
  HashMap* registers = (HashMap*)value;
  int pomodoro_duration = string_to_int(hashmap_get(g_pomodoros, key));
  
  printf("\n%s -> 🍅 = %d minutes\n\n", key, pomodoro_duration);
  hashmap_foreach(registers, process_register, (void*)&pomodoro_duration);
}


