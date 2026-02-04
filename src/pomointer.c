#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashmap.h"
#include "util.h"
#include "pomofile.h"
#include "export.h"

/*---------- CONSTANTS AND MACROS --------------*/

#define INITIAL_HASHMAP_SIZE 16
#define LOAD_FACTOR 0.75

/*---------- OPTION HANDLING RELATED -------------*/

typedef struct {
  bool aftdate_flag;
  bool befdate_flag;
  bool subj_flag;
  bool export_flag;
  time_t after_date;
  time_t before_date;
  char** subjects;
  char* export_type;
} Options;

static Options options = {false, false, false, false, -1, -1, NULL, NULL};

/*---------- GLOBAL VARIABLES --------------*/

static PomoFile* pomofiles_array = NULL;
static HashMap* filtered_registers = NULL;
static ProcessData process_data;

/*------------------------------------------------*/

/* Forward declarations */

static void clear_resources(void);
static void usage(void);
static int parse_options(int argc, char** argv);
static void validade_date_range(void);
static int initialize_pomofiles(int argc, int options_count);
static void handle_initialization_error(int processed_count, const char* filename);


static void clear_resources(void) {
  if (filtered_registers != NULL) {
    hashmap_destroy(filtered_registers, NULL);
    filtered_registers = NULL;
  }

  if (process_data.global_registers != NULL) {
    hashmap_destroy(process_data.global_registers, NULL);
    process_data.global_registers = NULL;
  }

  if (process_data.pomodoro_durations != NULL) {
    hashmap_destroy(process_data.pomodoro_durations, NULL);
    process_data.pomodoro_durations = NULL;
  }

  if (process_data.register_filter.subjects != NULL) {
    free_string_array(process_data.register_filter.subjects);
    process_data.register_filter.subjects = NULL;
  }

  if (pomofiles_array != NULL) {
    free(pomofiles_array);
    pomofiles_array = NULL;
  }

  process_data.register_filter.aftdate_flag = false;
  process_data.register_filter.befdate_flag = false;
  process_data.register_filter.subj_flag = false;
  process_data.register_filter.export_flag = false;
  process_data.register_filter.export_type = NULL;
  process_data.register_filter.after_date = -1;
  process_data.register_filter.before_date = -1;
}


static void usage(void) {
  fprintf(stderr, "Pomofile Interpreter\n"
                  "Usage: pomointer [OPTIONS] <pomofile1> <pomofile2> ... <pomofileN>\n\n"
                  "Options:\n"
                  "  -h                            Show this help message\n"
                  "  -a \"%%d/%%m/%%Y\"                 Filter entries after this date\n"
                  "  -b \"%%d/%%m/%%Y\"                 Filter entries before this date\n"
                  "  -s subj1,subj2,...,subjN      Filter entries by subject\n"
                  "  -e html                       Export to html file\n\n"
                  "Examples: \n"
                  "  pomointer -a \"16/01/2026\" -b \"31/01/2026\" pomofile1.pf pomofile2.pf\n"
                  "  pomointer -s \"Math,Physics\" -a \"05/03/2026\" -e \"html\" pomofile1.pf > index.html\n"
                  );
  exit(EXIT_FAILURE);
}


static void validade_date_range(void) {
  if (options.aftdate_flag && options.befdate_flag) {
    if (options.after_date > options.before_date) {
      fprintf(stderr, "Error: After date cannot be later than before date\n");
      exit(EXIT_FAILURE);
    }
  }
}


static int parse_options(int argc, char** argv) {
  int options_processed = 0;

  // Skip argv[0]
  for (int i = 1; i < argc; i++) {
    char* opt = argv[i];

    if (opt[0] != '-') {
      break; // No more options to parse
    }

    if (strcmp(opt, "-h") == 0) {
      usage();
    }
    else if (strcmp(opt, "-a") == 0 || strcmp(opt, "-b") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Error: option %s requires a date argument\n", opt);
        usage();
      }

      char* date_str = argv[i+1];
      time_t parsed_date = string_to_time(date_str);

      if (parsed_date == -1) {
        fprintf(stderr, "Error: Invalid date format for option '%s'\n", opt);
        fprintf(stderr, "Expected format: \"DD/MM/YYYY\"\n");
        exit(EXIT_FAILURE);
      }

      if (strcmp(opt, "-a") == 0) {
        options.aftdate_flag = true;
        options.after_date = parsed_date;
      } else {
        options.befdate_flag = true;
        options.before_date = parsed_date;
      }

      i++; // Skip the date argument
      options_processed += 2; // Flag and date argument
    }
    else if (strcmp(opt, "-s") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Error: option %s requires a list of subjects\n", opt);
        usage();
      }

      int count;
      char** subjects = split_string(argv[i+1], ',', &count);

      options.subj_flag = true;
      options.subjects = subjects;

      i++; // Skip the date argument
      options_processed += 2; // Flag and date argument
    }
    else if (strcmp(opt, "-e") == 0) {
      if (i + 1 >= argc || strcmp(argv[i+1], "html") != 0) {
        fprintf(stderr, "Error: option %s requires a valid file type to export\n", opt);
        usage();
      }

      options.export_flag = true;
      options.export_type = argv[i+1];
      
      i++; // Skip the export type argument
      options_processed += 2; // Flag and export type
    }
    else {
      fprintf(stderr, "Error: Unknown option '%s'\n", opt);
      usage();
    }
   }


  validade_date_range();
  return options_processed;
}


static void handle_initialization_error(int processed_count, const char* filename) {
  for (int i = 0; i < processed_count; i++) {
    free_pomofile(&pomofiles_array[i]);
  }
  clear_resources();
  fprintf(stderr, "Error: Failed to initialize PomoFile for '%s'\n", filename);
  exit(EXIT_FAILURE);
}


static int initialize_pomofiles(int argc, int options_count) {
  int num_files = argc - 1 - options_count;

  if (num_files <= 0) {
    fprintf(stderr, "Error: No input files specified\n");
    usage();
  }

  pomofiles_array = malloc(num_files * sizeof(PomoFile));
  if (pomofiles_array == NULL) {
    fprintf(stderr, "Error: Memory allocation failed to pomofiles array\n");
    exit(EXIT_FAILURE);
  }

  filtered_registers = hashmap_create(INITIAL_HASHMAP_SIZE, LOAD_FACTOR);
  process_data.global_registers = hashmap_create(INITIAL_HASHMAP_SIZE, LOAD_FACTOR);
  process_data.pomodoro_durations = hashmap_create(INITIAL_HASHMAP_SIZE, LOAD_FACTOR);

  // Copy options to data processing structure
  process_data.register_filter.aftdate_flag = options.aftdate_flag;
  process_data.register_filter.befdate_flag = options.befdate_flag;
  process_data.register_filter.after_date = options.after_date;
  process_data.register_filter.before_date = options.before_date;
  process_data.register_filter.subj_flag = options.subj_flag;
  process_data.register_filter.subjects = options.subjects;
  process_data.register_filter.export_flag = options.export_flag;
  process_data.register_filter.export_type = options.export_type;

  if (process_data.global_registers == NULL || process_data.pomodoro_durations == NULL) {
    fprintf(stderr, "Error: Failed to create hashmap structures\n");
    clear_resources();
    exit(EXIT_FAILURE);
  }

  return num_files;
}


int main(int argc, char** argv) {
  if (argc < 2) {
    usage();
  }

  // Parse command line options
  int options_count = parse_options(argc, argv);

  // Initialize structures
  int num_files = initialize_pomofiles(argc, options_count);

  // Initialize each pomofile
  int file_index = 1 + options_count; // Skip program name and options
  for (int i = 0; i < num_files; i++, file_index++) {
    int result = pomofile_init(&pomofiles_array[i], argv[file_index]);
    if (result != 0) {
      handle_initialization_error(i, argv[file_index]);
    }
  }

  // Parse all files
  for (int i = 0; i < num_files; i++) {
    parse_file(&pomofiles_array[i], process_data.global_registers, &process_data);
  }

  // Process global data
  filter_registers(&process_data, filtered_registers);
  HashMap** final_registers = NULL;

  if (hashmap_size(filtered_registers) == 0) {
    final_registers = &process_data.global_registers;
  } else {
    final_registers = &filtered_registers;
  }

  // Export to HTML
  if (options.export_flag && strcmp(options.export_type, "html") == 0) {
    print_html_top_part();
    hashmap_foreach(*final_registers, process_final_registers, &process_data);
    print_html_down_part();
  // Normal output
  } else {
    hashmap_foreach(*final_registers, process_final_registers, &process_data);
  }

  // Cleanup
  for (int i = 0; i < num_files; i++) {
    free_pomofile(&pomofiles_array[i]);
  }
  clear_resources();

  return EXIT_SUCCESS;
}
