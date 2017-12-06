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
    howderek_set_log_level(HOWDEREK_LOG_DEBUG);
    struct world* test = world_let_there_be_light(10);
    int i;
    for (i = 1; i < 11; i++) {
      position_t tmp;
      tmp.coordinates.x = i;
      tmp.coordinates.y = i;
      world_add(test, tmp);
    }
    howderek_graph_display(test->graph);
    position_t start;
    position_t end;
    start.coordinates.x = 1;
    start.coordinates.y = 1;
    end.coordinates.x = 10;
    end.coordinates.y = 10;
    struct pathfinding_data* list;
    struct howderek_graph_vertex* startV = world_at_position(test, start);
    struct howderek_graph_vertex* endV = world_at_position(test, end);
    list = astar(test->graph, startV, endV);
    while (list != NULL) {
      position_t pos;
      pos.bits = list->v->id;
      printf("x: %u, y: %u\n", pos.coordinates.x, pos.coordinates.y);
      list = list->next;
    }

    return 0;
}
