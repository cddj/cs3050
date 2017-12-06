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

    struct pathfinding_data* uData = u->data;
    struct pathfinding_data* vData = v->data;

    if (u->id == v->id) {
      return 0;
    } else {
      int8_t result = (uData->sumOfDistances == vData->sumOfDistances) ? 0 : ((uData->sumOfDistances >vData->sumOfDistances) ? 1 : -1);
      return result;
    }

  } else if (u != NULL && v != NULL) {
    return u->id - v->id;
  } else {
    return -1;
  }
}

int __calc_heuristic (struct howderek_graph_vertex* v1, struct howderek_graph_vertex* goal) {
   position_t v1Pos;
   v1Pos.bits = v1->id;
   position_t goalPos;
   goalPos.bits = goal->id;
   int64_t v1x = v1Pos.coordinates.x;
   int64_t v1y = v1Pos.coordinates.y;
   int64_t goalx = goalPos.coordinates.x;
   int64_t goaly = goalPos.coordinates.y;
   if(llabs(v1x - goalx) > llabs(v1y - goaly))
   {
        return llabs(v1x - goalx);
   }
   else
   {
       return llabs(v1y - goaly);
   }
}

int __is_goal (struct howderek_graph_vertex* v1, struct howderek_graph_vertex* goal) {
    position_t v1Pos;
    v1Pos.bits = v1->id;
    position_t goalPos;
    goalPos.bits = goal->id;
    if (v1Pos.coordinates.x == goalPos.coordinates.x && v1Pos.coordinates.y == goalPos.coordinates.y) {
        return 1;
    }
    return 0;
}

struct pathfinding_data* find_distance (struct howderek_graph* g, struct howderek_graph_vertex* startVertex, struct howderek_graph_vertex* endVertex) {
    const enum howderek_vertex_status unvisitedColor = HOWDEREK_GRAPH_COLOR(startVertex->status);
    const enum howderek_vertex_status visitedColor = (unvisitedColor == HOWDEREK_GRAPH_BLACK) ? HOWDEREK_GRAPH_WHITE : HOWDEREK_GRAPH_BLACK;
    struct pathfinding_data* closedList;
    struct pathfinding_data* closedListEnd = NULL;
    struct howderek_heap* openList = howderek_heap_create(0, __howderek_graph_sum_compare);
    struct pathfinding_data* curr;
    struct pathfinding_data* tmp;
    struct howderek_graph_edge* edge;
    struct pathfinding_data* start = malloc(sizeof(struct pathfinding_data));
    start->v = startVertex;
    start->distance = 0;
    start->heuristicDistance = __calc_heuristic(start->v, endVertex);
    start->sumOfDistances = start->distance + start->heuristicDistance;
    start->next = NULL;

    howderek_heap_push(openList, start);

    while (openList->store->size != 0) {
        curr = howderek_heap_pop(openList);

        if (closedListEnd != NULL) {
            closedListEnd->next = curr;
            closedListEnd = closedListEnd ->next;
            closedListEnd->next = NULL;
        } else {
            closedList = curr;
            closedListEnd = curr;
        }

        HOWDEREK_GRAPH_SET_COLOR(curr->v->status, visitedColor);
        edge = curr->v->edges;

        while (edge != NULL) {
            if (__is_goal(edge ->vertex, endVertex)) {
                closedListEnd->next = malloc(sizeof(struct pathfinding_data));
                closedListEnd = closedListEnd->next;
                closedListEnd->distance = curr->distance + 1;
                closedListEnd->heuristicDistance = 0;
                closedListEnd->sumOfDistances = closedListEnd ->distance + closedListEnd->heuristicDistance;
                closedListEnd->v = edge ->vertex;
                closedListEnd->next = NULL;
                return closedList;
            } else if (HOWDEREK_GRAPH_COLOR(edge->vertex->status) != visitedColor) {
                tmp = malloc(sizeof(struct pathfinding_data));
                tmp->distance = curr ->distance + 1;
                tmp->heuristicDistance = __calc_heuristic(edge->vertex, endVertex);
                tmp->sumOfDistances = tmp->distance + tmp->heuristicDistance;
                tmp->v = edge ->vertex;
                tmp->next = NULL;
                howderek_heap_push(openList, tmp);
            }
            edge = edge->next;
        } // end while (edge)
    } // end while (open list)
    return NULL;
}
