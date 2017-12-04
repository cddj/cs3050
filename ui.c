/*! \file ui.c
 *  \brief Handles user interaction with the program. Displays a world.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "libhowderek/howderek.h"
#include "libhowderek/howderek_hashmap.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_kv.h"




/*!
 *
 *  1. Print to console the steps taken by the robots in the world. One by one.
 *
 * Load graph into 2 dimensional array.
 * iterate through 2 dimensional array, printing it out.
 *
 * */

void renderer(struct world* w) {

    // Not yet implemented.

    //Variables etc.
    int i,j;
    char** worldArray = malloc(sizeof(char)*100);
    *worldArray = malloc(sizeof(char)*100);

    //loads the world into a 2 dimensional array
    for(i = 0;i< 100;i++){
        for(j = 0; j< 100 ;j++){
            struct howderek_kv_iter iter;
            howderek_kv_fill_iter(w->graph.vertex_map, &iter);
            while (howderek_kv_iterate(w->graph.vertex_map, &iter)) {
            howderek_graph_display_vertex_info(iter.value, NULL);

            worldArray[i][j] = iter.value;

            }
        }
    }


    //printing the 2 dimensional array
    for(i = 0;i< 100;i++){
        for(j = 0; j< 100 ;j++){
            printf("%c",worldArray[i][j]);
        }
    }
    //w->graph.vertex_map->array[i]
    //howderek_graph_display(&(w->graph));

    //Clears space inbetween, if I am to believe what Derek says
    howderek_clear();
  return;
}
