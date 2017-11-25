/*! \file libderek.h
    \brief Common functions used across my homework assignments.
*/

#ifndef H_LIBHOWDEREK_H
#define H_LIBHOWDEREK_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

// Terminal escape codes
#define HOWDEREK_INVERSE   "\x1b[7m"
#define HOWDEREK_PURPLE    "\e[0;35m"
#define HOWDEREK_RED       "\x1b[31m"
#define HOWDEREK_YELLOW    "\x1b[33m"
#define HOWDEREK_GREEN     "\x1b[32m"
#define HOWDEREK_BLUE      "\x1b[34m"
#define HOWDEREK_CYAN      "\x1b[36m"
#define HOWDEREK_BOLD      "\x1b[1m"
#define HOWDEREK_RESET     "\x1b[0m"
#define HOWDEREK_CLEAR     "\033[2J\033[1;1H"


/** log_level_t defines which log level should be displayed to stderr. */
typedef enum {
  HOWDEREK_LOG_SILENT    = 0,
  HOWDEREK_LOG_FATAL     = 1,
  HOWDEREK_LOG_ERROR     = 2,
  HOWDEREK_LOG_SUCCESS   = 3,
  HOWDEREK_LOG_WARN      = 4,
  HOWDEREK_LOG_INFO      = 5,
  HOWDEREK_LOG_VERBOSE   = 6,
  HOWDEREK_LOG_DEBUG     = 7,
  HOWDEREK_LOG_SET_LEVEL =  100
} howderek_log_level_t;

/** log_level_t defines which log level should be displayed to stderr. */
struct howderek_configuration_t {
  FILE* in;
  FILE* out;
  howderek_log_level_t logLevel;
};


/**
 * Manual memory management to reduce mallocs for programs
 * that would otherwise malloc very quickly
 * 
 * \param level     Log level to display
 */
void howderek_set_log_level(howderek_log_level_t level);

/**
 * Logs an error
 *
 * \param level     Log level (ERROR, WARN, INFO, VERBOSE, DEBUG) of the error
 * \param message   Error message
 */
howderek_log_level_t howderek_log(howderek_log_level_t level, const char* message, ...);

/**
 * Sets what level should be logged
 *
 * \param level     Log level to display
 */
void howderek_set_log_level(howderek_log_level_t level);

/**
 * Clears the screen
 */
void howderek_clear();

/**
 * Menu
 * 
 * \param promptString   the menu prompt
 * \param failString     the string to display if something filed
 * \param min            the minimum accepted option
 * \param max            the maximum accepted option
 */
int howderek_menu(char* promptString, char* failString, const int min, const int max);

/**
 * Open input/output files
 * 
 * \param argc  argc from main
 * \param argv  argv from main
 */
struct howderek_configuration_t howderek_parse_command_line_options(int argc, char *argv[]);

/**
 * Close config files
 * 
 * \param config  config with files to close
 */
void howderek_clean_up_config(struct howderek_configuration_t* config);

/**
 * Sets the time in miliseconds since its last call
 */
size_t howderek_global_clock(void);


// hand-picked artisan primes
static const uint64_t howderek_artisan_primes[] = {
  /*********************************************************|
  |             __            __               ___          |
  |        ___ / /  ___ _____/ /__ ___  ___  _/  /__        |
  |       (_-</ _ \/ _ `/ __/  '_// _ \/ _ `/ _ / _ \       |
  |      /___/_//_/\_,_/_/ /_/\_\/_//_/\_,_/\__/\___/       |
  |     ### ### # # ### # # ### # # ### # # ### ### ###     |
  |       #   # # #   # # #   # # #   # # #   # # # # #     |
  |     ### ### ### ### ### ### ### ### ### ### ### ###     |
  |     #   #     # #     # #     # #     # #     #   #     |
  |     ### ###   # ###   # ###   # ###   # ### ### ###     |
  |                                                         |
  |_________________________________________________________|
  |  |_  \       \\\\\\\\\\\\\//////////////    \       \   |
  |                \\\\\\\\\\\/////////.                    |
  |                 \\\\\\\\\///////               \        |
  |       \    \     \\\\\\\/////             \             |
  |                     \\\\\\\\//                          |
  |                        \\\\//                     \     |
  |         \    \          \\\/               \            |
  |     \                   \\//                   \   \    |
  |             \           \|/                            */
  3,         7,         17,  /**/   31,         73,        //
  179,       419,       947,/*****/ 1597,       3187,      //
  6373,      13313,     27697,      53597,      101107,    //
  202751,/**/404843,/**/811123,/**/ 1609999,/**/3211777,   //
  6410777,   12811177,  25642717,   51255137,   102411119, //
  204820003, 409641773, 918282011,  1836284609, 3636284777,//
  7236293777,           14436285713,            28842424243,
  56842424699,          112242424717,           224242424299,
  424242424609,
  /*********************************************************|
  |                                                         |
  |              Starring the Mersenne Twister              |
  |                                                         |
  |*********************************************************/
};
static const size_t howderek_artisan_prime_count =
    sizeof(howderek_artisan_primes) /
    sizeof(howderek_artisan_primes[0]);

/**
 * Return the size it should be to fit a given number of elements
 */
size_t howderek_optimal_prime(size_t count);

#endif
