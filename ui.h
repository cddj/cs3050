/*! \file ui.h
 *  \brief Handles user interaction with the program. Displays a world.
 */

#include "world.h"
#include "libhowderek/howderek_graph.h"


struct pixel {
    char c;
    enum howderek_vertex_status color;
};


/**
 * Render the world. To be passed as the renderer to world_simulate in world.h
 *
 * \param w       the world
 */
void renderer(struct world* w);
