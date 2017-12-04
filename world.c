/*! \file world.c
 *  \brief The world in which the robots live. Wraps a howderek_graph with
 *         functions accesible to robots to ensure they don't violate the
 *         constraints of the assignment. Robots are completely aware of the
 *         world, including the entire graph and the other robots.
 */

#include "libhowderek/howderek.h"
#include "libhowderek/howderek_graph.h"
#include "libhowderek/howderek_hashmap.h"
#include "world.h"


const char* DIRECTION_STRINGS[] = {
  "N ",
  "NE",
  "E ",
  "SE",
  "S ",
  "SW",
  "W ",
  "NW"
};


struct world* world_let_there_be_light(size_t size) {
  if (size == 0) {
    size = 128;
  }
  struct world* creation = malloc(sizeof(struct world));
  creation->graph = howderek_graph_create(HOWDEREK_GRAPH_UNDIRECTED, 128);
  return creation;
}

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
                                 direction_t direction) {
  position_t newPosition;
  newPosition.bits = starting.bits;
  int64_t offsetX, offsetY;
  switch (direction) {
    case N:
      offsetX = 0;
      offsetY = 1;
      break;
    case NE:
      offsetX = 1;
      offsetY = 1;
      break;
    case E:
      offsetX = 1;
      offsetY = 0;
      break;
    case SE:
      offsetX = 1;
      offsetY = -1;
      break;
    case S:
      offsetX = 0;
      offsetY = -1;
      break;
    case SW:
      offsetX = -1;
      offsetY = -1;
      break;
    case W:
      offsetX = -1;
      offsetY = 0;
      break;
    case NW:
      offsetX = -1;
      offsetY = 1;
      break;
  }
  offsetX *= distance;
  offsetY *= distance;
  newPosition.coordinates.x += offsetX;
  newPosition.coordinates.y += offsetY;
  return newPosition;
}


/**
 * Return vertex or NULL (if doesn't exist)
 *
 * \param w           the world
 * \param pos         position to find a node
 *
 * \return            vertex at that position or NULL
 */
struct howderek_graph_vertex* world_at_position(struct world* w,
                                                position_t pos) {
  return howderek_graph_get(w->graph, pos.bits);
}

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
                                        position_t pos) {
  position_t* newPos = malloc(sizeof(position_t));
  newPos->coordinates.x = pos.coordinates.x;
  newPos->coordinates.y = pos.coordinates.y;
  struct howderek_graph_vertex* v = howderek_graph_add_vertex(w->graph, pos.bits, NULL);
  for (direction_t currentDirection = N; currentDirection < NE; currentDirection++) {
    struct howderek_graph_vertex* testVertex = world_at_position(w, world_line_from(pos, 1, currentDirection));
    if (testVertex != NULL) {
      howderek_graph_add_edge(w->graph, v, testVertex, 1.0);
    }
  }
}


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
