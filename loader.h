/*! \file loader.h
 *  \brief Loads a world from a file
 */
#pragma once

#include <stdio.h>
#include "robot.h"
#include "world.h"
#include "libhowderek/howderek.h"
#include "libhowderek/howderek_graph.h"

#define NUMBER_OF_ROBOTS 2

struct world {
    struct howderek_graph graph;
    struct robot robots[NUMBER_OF_ROBOTS];
};


/**
 * Create a heap
 *
 * \param size      number of elements it should hold by default.
 */
int load_world(struct world* theWorld, FILE* file);
