/*! \file robot.c
 *  \brief Implementation of the robot AI.
 *         Moves about a world and communicates with other robots in that world
 *         to avoid collisions.
 */

#include <math.h>
#include "libhowderek/howderek.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_array.h"
#include "libhowderek/howderek_heap.h"
#include "world.h"

int8_t __howderek_graph_sum_compare(howderek_array_value_t left, howderek_array_value_t right) {
  struct howderek_graph_vertex* u = left;
  struct howderek_graph_vertex* v = right;

  if (u != NULL &&
      v != NULL &&
      u->id != HOWDEREK_HASHMAP_NULL_ID &&
      v->id != HOWDEREK_HASHMAP_NULL_ID &&
      u->data != NULL &&
      v->data != NULL) {

    struct world_pathfinding_data* uData = u->data;
    struct world_pathfinding_data* vData = v->data;

    if (u->id == v->id) {
      return 0;
    } else {
      int8_t result = (uData->sumOfDistances == vData->sumOfDistances) ? 0 : ((uData->sumOfDistances > vData->sumOfDistances) ? 1 : -1);
      return result;
    }

  } else if (u != NULL && v != NULL) {
    return u->id - v->id;
  } else {
    return -1;
  }
}

int __calc_heuristic(struct howderek_graph_vertex* v1, struct howderek_graph_vertex* goal)
{
//   return max(abs(v1->data->x - goal->data->x), abs(v1->data->y - goal->data->y));
     return 0;
}

int __is_goal(struct howderek_graph_vertex* start_vertex, struct howderek_graph_vertex* goal)
{
    return 0;
}

struct world_pathfinding_data* find_distance(struct howderek_graph g, struct howderek_graph_vertex* start_vertex, struct howderek_graph_vertex* goal)
{
    struct world_pathfinding_data* closed_list;
    struct world_pathfinding_data* closed_list_end;
    struct howderek_heap* open_list = howderek_heap_create(0, __howderek_graph_sum_compare);
    struct world_pathfinding_data* curr;
    struct howderek_graph_edge* edge;
    int tmp_g; // distance to start
    int tmp_h; // est distance to goal
    int tmp_sum;
    struct world_pathfinding_data* start = malloc(sizeof(struct world_pathfinding_data));
    start ->v = start_vertex;
    start ->distance = 0;
    start ->heuristicDistance = __calc_heuristic(start->v, goal);

    howderek_heap_push(open_list, start);

    while(open_list -> store -> size != 0)
    {
        curr = howderek_heap_pop(open_list);
        edge = curr->v->edges;
        while(edge != NULL)
        {
            if(__is_goal(edge->vertex, goal))
            {
                closed_list_end -> next = malloc(sizeof(struct world_pathfinding_data));
                closed_list_end = closed_list -> next;
                closed_list_end -> distance = curr->distance + 1;
                closed_list_end -> heuristicDistance = 0;
                closed_list_end -> sumOfDistances = closed_list_end -> distance + closed_list_end -> heuristicDistance;
                closed_list_end -> v = edge -> vertex;
                closed_list_end -> next = NULL;
                return closed_list;
            }


            edge = edge->next;
        }

    }

    return NULL;

}

