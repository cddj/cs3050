/*! \file robot.c
 *  \brief Implementation of the robot AI.
 *         Moves about a world and communicates with other robots in that world
 *         to avoid collisions.
 */

#include "libhowderek/howderek.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_array.h"
#include "world.h"

struct world_pathfinding_data* find_distance(struct howderek_graph g, struct howderek_graph_vertex start, struct howderek_vertex end)
{
    struct howderek_array* closed_list = howderek_array_create(0);
    struct howderek_heap* open_list = howderek_heap_create(0);
}
