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


//int main(int argc, char** argv) {
//  struct howderek_configuration_t config = howderek_parse_command_line_options(argc, argv);
//  howderek_log(HOWDEREK_LOG_INFO, "Loading...");
//  struct world* w = load_world(config.in);
//  return 0;
//}

int main(void)
{
    struct world* test = world_let_there_be_light(90);
    position_t* tmp;
    int i;
    for (i = 1; i < 11; i++) {
        tmp = malloc(sizeof(position_t));
        tmp->coordinates.x = i;
        tmp->coordinates.y = 0;
        world_add(test, *tmp);
    }

    position_t* start = malloc(sizeof(position_t));
    position_t* end = malloc(sizeof(position_t));
    start->coordinates.x = 1;
    start->coordinates.y = 0;
    end->coordinates.x = 10;
    end->coordinates.y = 0;
    struct pathfinding_data* list;
    list = find_distance(test->graph, world_at_position(test, *start), world_at_position(test, *end));
    while (list != NULL) {
      position_t *pos = list->v->data;
      printf("x: %u, y: %u", pos->coordinates.x, pos->coordinates.y);
    }

    return 0;
}
