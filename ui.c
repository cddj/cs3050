/*! \file ui.c
 *  \brief Handles user interaction with the program. Displays a world.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "libhowderek/howderek.h"
#include "libhowderek/howderek_hashmap.h"
#include "libhowderek/howderek_graph.h"




/*!
 *
 *  1. Print to console the steps taken by the robots in the world. One by one.
 *
 * */

void renderer(struct world* w) {


    // Not yet implemented.

    //w->graph.vertex_map->array[i]
    howderek_graph_display(&(w->graph));


    //howderek_clear();
  return;
}
