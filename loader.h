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

/**
 * Load a world from a file
 *
 * \param file    the file to load
 * \return        the world created
 */
struct world* load_world(FILE* file);
