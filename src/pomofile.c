#include <stdio.h>
#include <string.h>
#include "util.h"
#include "pomofile.h"
#include "preprocessor.h"
#include "process_data.h"

/* -------------------------- AUXILIARY FUNCTIONS DECLARATIONS -------------------------------- */

static void print(const char* key, void* value, void* type);
static int merge_pomodoros_counters(HashMap* old_counters, HashMap* updated_counters);
static int search_abbvr(HashMap* assignments, HashMap* registers);
static void process_register(const char* date, void* pomodoros_ammount, void* pomodoro_duration);

static int read_assignment(char* line, HashMap* assignments);
static int read_register(char* line, HashMap* registers);
static LineType classify_line(char* line);

static bool is_pomodoro_duration_defined(HashMap* assignments);
static bool is_date_defined(HashMap* assignments);

static int get_pomodoro_duration(HashMap* assignments);
static time_t get_date(HashMap* assignments);

/* ---------------------------------- AUXILIARY FUNCTIONS ------------------------------------ */

static void print(const char* key, void* value, void* type) {
  if (strcmp("ASSIGNMENT", (const char*)type) == 0) {
    printf("\"%s\":\"%s\",\n", key, (char*)value);
  } else {
    int pomodoros_ammount = string_to_int((char*)value);
    printf("\"%s\":\"%d\",\n", key, pomodoros_ammount);
  }
}

static int merge_pomodoros_counters(HashMap* old_counters, HashMap* updated_counters) {
  if (!old_counters || !updated_counters ) return 0;
  int added_elements = 0;
  for (int i = 0; i < updated_counters->capacity; i++) {
    Entry* entry = updated_counters->buckets[i];
    while (entry) {
      int current_value = string_to_int(hashmap_get(old_counters, entry->key));

      if (current_value) {
        int new_value = current_value + string_to_int(entry->value);
        hashmap_put(old_counters, entry->key, int_to_string(new_value));
      } else {
        hashmap_put(old_counters, entry->key, entry->value);
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

static void process_register(const char* date, void* pomodoros_ammount, void* pomodoro_duration) {
  printf("%s:\n", date);
  int p_ammount = string_to_int(pomodoros_ammount);
  int duration = *(int*)pomodoro_duration;
  for (int i = 0; i < p_ammount; i++) {
    printf("🍅");
  }
  printf(" -> %d minutes\n", p_ammount * duration);
}

static LineType classify_line(char *line) {
  // Skips initial whitespaces
  line = trim_left(line);
  
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
  int parts_count;
  char** parts = split_string(line, '=', &parts_count);

  if (parts && parts_count == 2) {
    char* subject_name_abbreviation = strip_string(parts[0], NULL);
    char* subject_name = strip_string(parts[1], NULL);

    hashmap_put(assignments, subject_name_abbreviation, subject_name);
    free_string_array(parts);

    return 1;
  }

  free_string_array(parts);
  return 0;
}

static int read_register(char *line, HashMap *registers) {
  int parts_count;
  char** parts = split_string(line, ':', &parts_count);
  
  if (parts && parts_count == 2) {
    char* subject = strip_string(parts[0], NULL);
    char* stars_string = strip_string(parts[1], NULL);

    // If it exists, update its value
    if (hashmap_get(registers, subject) != NULL) {
      int old_pomodoros_ammount = string_to_int(hashmap_get(registers, subject));
      int current_pomodoros_ammount = count_stars(stars_string);
      int updated_pomodoros_ammount = old_pomodoros_ammount + current_pomodoros_ammount;
      hashmap_put(registers, subject, int_to_string(updated_pomodoros_ammount));
    } else {
      // If not, create it
      hashmap_put(registers, subject, int_to_string(count_stars(stars_string)));
    }

    free_string_array(parts);
    return 1;
  }

  free_string_array(parts);
  return 0;
}

static bool is_pomodoro_duration_defined(HashMap* assignments) {
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
  int minutes = string_to_int(hashmap_get(assignments, "POMO"));

  return minutes;
}

static bool is_date_defined(HashMap* assignments) {
  bool exists = hashmap_contains(assignments, "DATE");

  if (!exists) {
    return false;
  }

  char* date = (char*)hashmap_get(assignments, "DATE");
  if (string_to_time(date) == -1) {
    return false;
  }

  return true;
}

static time_t get_date(HashMap* assignments) {
  char* date = hashmap_get(assignments, "DATE");
  return string_to_time(date);
}

/* ---------------------------------- AUXILIARY FUNCTIONS END ---------------------------------- */

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

int parse_file(PomoFile* pomofile, HashMap* global_registers, ProcessData* process_data) {
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

  if (is_pomodoro_duration_defined(pomofile->assignments)) {
    pomofile->pomodoro_duration = get_pomodoro_duration(pomofile->assignments);
  }
  if (is_date_defined(pomofile->assignments)) {
    pomofile->date = get_date(pomofile->assignments);
  }

  search_abbvr(pomofile->assignments, pomofile->registers);

  // If there's no entry in global registers, create new
  if (hashmap_get(global_registers, time_to_string(pomofile->date)) == NULL) {
    hashmap_put(global_registers, time_to_string(pomofile->date), (void*)pomofile->registers);
  } else {
    // If it already exists, update it
    HashMap* old_counters = hashmap_get(global_registers, time_to_string(pomofile->date)); 
    HashMap* updated_counters = pomofile->registers;

    merge_pomodoros_counters(old_counters, updated_counters);
  }

  // Pomodoro duration for that day
  hashmap_put(process_data->pomodoro_durations, time_to_string(pomofile->date), int_to_string(pomofile->pomodoro_duration));

  fclose(f); 
  return 1;
}

void process_global_registers(const char* date, void* registers, void* process_data) {
  ProcessData* proc_data = (ProcessData*)process_data;
  //RegisterFilter* register_filter = &proc_data->register_filter;

  HashMap* pomodoros_durations = proc_data->pomodoro_durations;
  HashMap* regs = (HashMap*)registers;
  int pomodoro_duration = string_to_int(hashmap_get(pomodoros_durations, date));
  
  printf("\n%s -> 🍅 = %d minutes\n\n", date, pomodoro_duration);
  hashmap_foreach(regs, process_register, &pomodoro_duration);
}


