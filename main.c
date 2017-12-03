/*! \file main.c
    \brief Skiplist implemenetation by Derek Howard
*/
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#include "libhowderek/howderek.h"
#include "world.h"
#include "robot.h"
#include "loader.h"
#include "ui.h"

/* main

8b    d8    db    88 88b 88
88b  d88   dPYb   88 88Yb88
88YbdP88  dP__Yb  88 88 Y88
88 YY 88 dP""""Yb 88 88  Y8

*/


int main(int argc, char** argv) {
  struct howderek_configuration_t config = howderek_parse_command_line_options(argc, argv);
  howderek_log(HOWDEREK_LOG_INFO, "Loading...");
  struct world* w = load_world(config.in);
  return 0;
}
