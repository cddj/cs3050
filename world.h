/*! \file world.h
 *  \brief The world in which the robots live. Wraps a howderek_graph with
 *         functions accesible to robots to ensure they don't violate the
 *         constraints of the assignment. Robots are completely aware of the
 *         world, including the entire graph and the other robots.
 */

#pragma once

struct world {
    struct howderek_graph graph;
    struct robot robots[NUMBER_OF_ROBOTS];
};
