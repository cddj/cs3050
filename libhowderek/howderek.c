#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>

#include "howderek.h"


howderek_log_level_t howderek_log(howderek_log_level_t level, const char* format, ...) {

  static howderek_log_level_t logLevel = HOWDEREK_LOG_INFO;
  char* prefix;
  switch (level) {
    case HOWDEREK_LOG_SILENT:
      return logLevel;
    case HOWDEREK_LOG_FATAL:
      prefix = HOWDEREK_BOLD HOWDEREK_RED "fatal   " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_ERROR:
      prefix = HOWDEREK_RED "error " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_SUCCESS:
      prefix = HOWDEREK_GREEN "success " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_WARN:
      prefix = HOWDEREK_YELLOW "warn  " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_INFO:
      prefix = "info";
      break;
    case HOWDEREK_LOG_VERBOSE:
      prefix = HOWDEREK_BLUE "verbose " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_DEBUG:
      prefix = HOWDEREK_CYAN "debug " HOWDEREK_RESET;
      break;
    case HOWDEREK_LOG_SET_LEVEL:
    default:
      logLevel = level - HOWDEREK_LOG_SET_LEVEL;
      return logLevel;
  }

  if (level <= logLevel && format != NULL) {
    //Get the time
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestring[64];
    strftime(timestring, sizeof(timestring), "%c", tm);

    //Time since last log
    static clock_t start = 0;
    size_t result = (clock() - start)/(CLOCKS_PER_SEC / 1000);
    start = clock();

    fprintf(stderr, "%-17s - %s - %-10lu ", prefix, timestring, result);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
  }

  return logLevel;

}

void howderek_set_log_level(howderek_log_level_t level) {
  howderek_log(HOWDEREK_LOG_SET_LEVEL + level, NULL);
}

void howderek_clear() {
  printf(HOWDEREK_CLEAR);
}

int howderek_menu(char* promptString, char* failString, const int min, const int max) {
  int selection = 0;
  //Loop forever until user enters a sensible Node
  for (;;) {
    printf("%s", promptString);
    scanf("%d", &selection);
    if (selection >= min && selection <= max) {
      return selection;
    }
    //Print fail string since the user didn't enter a Node
    howderek_log(HOWDEREK_LOG_ERROR, failString);
  }
}

struct howderek_configuration_t howderek_parse_command_line_options(int argc, char *argv[]) {
  howderek_global_clock(); //Start the clock when the program starts

  if (argc < 2) {
    howderek_log(HOWDEREK_LOG_FATAL, "Input and output file must be passed.");
    exit(EINVAL);
  }

  struct howderek_configuration_t config;

  config.logLevel = HOWDEREK_LOG_WARN;

  int flagOffset = 0;

  if (strcmp(argv[1], "--debug") == 0 || strcmp(argv[1], "-vv") == 0) {
    flagOffset++;
    howderek_set_log_level(HOWDEREK_LOG_DEBUG);
    config.logLevel = HOWDEREK_LOG_DEBUG;
  }

  if (strcmp(argv[1], "--verbose") == 0 || strcmp(argv[1], "-v") == 0) {
    flagOffset++;
    howderek_set_log_level(HOWDEREK_LOG_VERBOSE);
    config.logLevel = HOWDEREK_LOG_VERBOSE;
  }

  if (strcmp(argv[1], "--silent") == 0) {
    flagOffset++;
    howderek_set_log_level(HOWDEREK_LOG_SILENT);
    config.logLevel = HOWDEREK_LOG_SILENT;
  }

  

  config.in = fopen(argv[1 + flagOffset], "r");
  if (config.in == NULL) {
    howderek_log(HOWDEREK_LOG_FATAL, "%s could not be opened as the input file.", argv[1 + flagOffset]);
    exit(EACCES);
  }

  config.out = fopen(argv[2 + flagOffset], "w");
  if (config.out == NULL) {
    howderek_log(HOWDEREK_LOG_FATAL, "%s could not be opened as the output file.", argv[2 + flagOffset]);
    exit(EACCES);
  }

  return config;
}


void howderek_clean_up_config(struct howderek_configuration_t* config) {
  int error;
  error = fclose(config->in);
  if (error != 0) {
    howderek_log(HOWDEREK_LOG_FATAL, "Could not close input file.");
    exit(EBADF);
  }
  error = fclose(config->out);
  if (error != 0) {
    howderek_log(HOWDEREK_LOG_FATAL, "Could not close output file.");
    exit(EBADF);
  }
}

size_t howderek_global_clock() {
  static clock_t start = 0;
  size_t result = (clock() - start)/(CLOCKS_PER_SEC / 1000);
  start = clock();
  return result;
}

/**
 * Return the size it should be to fit a given number of elements
 */
size_t howderek_optimal_prime(size_t count) {
  count = (count / 3) + 1;
  size_t i;
  for (i = 0; i < howderek_artisan_prime_count; i++) {
    if (count < (howderek_artisan_primes[i] - (howderek_artisan_primes[i] / 10))) {
      break;
    }
  }
  return i;
}
