/*! \file robot.c
 *  \brief Implementation of the robot AI.
 *         Moves about a world and communicates with other robots in that world
 *         to avoid collisions.
 */

#include "libhowderek/howderek.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_array.h"
#include "libhowderek/howderek_heap.h"
#include "world.h"

int8_t __howderek_graph_weighted_distance_compare(howderek_array_value_t left, howderek_array_value_t right) {
  struct howderek_graph_vertex* u = left;
  struct howderek_graph_vertex* v = right;

  if (u != NULL &&
      v != NULL &&
      u->id != HOWDEREK_HASHMAP_NULL_ID &&
      v->id != HOWDEREK_HASHMAP_NULL_ID &&
      u->data != NULL &&
      v->data != NULL) {

    struct howderek_graph_pathfinding_data* uData = u->data;
    struct howderek_graph_pathfinding_data* vData = v->data;

    if (u->id == v->id) {
      return 0;
    } else {
      int8_t result = (uData->weightedDistance == vData->weightedDistance) ? 0 : ((uData->weightedDistance > vData->weightedDistance) ? 1 : -1);
      return result;
    }

  } else if (u != NULL && v != NULL) {
    return u->id - v->id;
  } else {
    return -1;
  }
}


struct world_pathfinding_data* find_distance(struct howderek_graph g, struct howderek_graph_vertex* start, struct howderek_graph_vertex* end)
{
    struct howderek_array* closed_list = howderek_array_create(0);
    struct howderek_heap* open_list = howderek_heap_create(0, __howderek_graph_weighted_distance_compare);
    struct howderek_graph_vertex* curr;

    howderek_heap_push(open_list, start);

    while(open_list -> store -> size != 0)
    {
        curr = howderek_heap_pop(open_list);
    }

}
