/*! \file ui.h
 *  \brief Handles user interaction with the program. Displays a world.
 */

#include "world.h"

/**
 * Render the world. To be passed as the renderer to world_simulate in world.h
 *
 * \param w       the world
 */
void renderer(struct world w);
