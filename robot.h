/*! \file robot.c
 *  \brief Implementation of the robot AI.
 *         Moves about a world and communicates with other robots in that world
 *         to avoid collisions.
 */

#pragma once

#include "libhowderek/howderek_graph.h"

struct robot {
    struct howderek_graph_vertex* pos;
    struct howderek_graph_vertex* goal;
};

struct world_pathfinding_data* find_distance(struct howderek_graph g, struct howderek_graph_vertex* start, struct howderek_graph_vertex* end);
