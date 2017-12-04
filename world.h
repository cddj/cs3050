/*! \file world.h
 *  \brief The world in which the robots live. Wraps a howderek_graph with
 *         functions accesible to robots to ensure they don't violate the
 *         constraints of the assignment. Robots are completely aware of the
 *         world, including the entire graph and the other robots.
 */

#pragma once

#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_hashmap.h"
#include "robot.h"

#define NUMBER_OF_ROBOTS 2

struct world {
    struct howderek_graph* graph;
    struct robot robots[NUMBER_OF_ROBOTS];
};

typedef union {
  struct {
      uint32_t x;
      uint32_t y;
  } coordinates;
  uint64_t bits;
} position_t;

struct world_pathfinding_data
{
    size_t distance;
    int heuristicDistance;
    int sumOfDistances;
    struct howderek_graph_vertex* v;
    struct world_pathfinding_data* next;
};

typedef enum {
  N,
  NE,
  E,
  SE,
  S,
  SW,
  W,
  NW
} direction_t;

/**
 * Create a world
 *
 * \return            all of creation (the world)
 */
struct world* world_let_there_be_light(size_t size);

/**
 * Calulate a new position given a distance and direction
 *
 * \param starting    inital position
 * \param distance    distance, can be positive or negative, must be < UINT32_MAX
 * \param direction   direction to move in
 *
 * \return            the calculated position
 */
position_t world_line_from(position_t starting,
                                int64_t distance,
                                direction_t direction);


/**
 * Return vertex or NULL (if doesn't exist)
 *
 * \param w           the world
 * \param pos         position to find a node
 *
 * \return            vertex at that position or NULL
 */
struct howderek_graph_vertex* world_at_position(struct world* w,
                                                position_t pos);

/**
 * Add a space that a robot can reach in the world. Automaticall connects to
 * nearby spaces
 *
 * \param starting    inital position
 * \param distance    distance, can be positive or negative, must be < UINT32_MAX
 * \param direction   direction to move in
 *
 * \return            the vertex created
 */
struct howderek_graph_vertex* world_add(struct world* w,
                                        position_t pos);


/**
 * Remove a space from a world
 *
 * \param w          the world
 * \param pos        position to remove
 *
 * \return           1 if successful, 0 if failed
 */
int world_remove(struct world* w,
                 position_t pos);

/**
 * Clone a world
 *
 * \param world       the world
 *
 * \return            the clone of the world
 */
struct world* world_clone(struct world* w, position_t pos);


/**
 * Clone a world
 *
 * \param world       the world
 *
 * \return            the clone of the world
 */
struct howderek_graph_vertex* world_adjacent(struct howderek_graph_vertex* vertex);


/**
 * Run a simulation of the world
 *
 * \param world       the world to simulate
 * \param renderer    function that renders the world
 *
 * \return 1 if a solution was found, 0 if a solution does not exist
 */
int world_simulate(struct world* w,
                   void(*renderer)(struct world* w));
