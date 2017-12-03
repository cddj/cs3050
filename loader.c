/*! \file loader.c
 *  \brief Loads a world from a file
 */

#include <errno.h>
#include "loader.h"
#include "world.h"

struct world* load_world(FILE* file) {
  struct world* w = malloc(sizeof(struct world));
  // Load everything into w here.
  return w;
}
