/*! \file howderek_heap.h
    \brief d-heap
*/

#ifndef H_LIBHOWDEREK_HEAP_H
#define H_LIBHOWDEREK_HEAP_H

#ifndef HOWDEREK_HEAP_CONFIG
// Default to a 4-heap
#define HOWDEREK_HEAP_DEFAULT_D 4
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_array.h"

#define HOWDEREK_HEAP_PARENT_INDEX(heap, i) ((i == 0) ? 0 : ((i-1)/heap->childrenPerNode))
#define HOWDEREK_HEAP_PARENT(heap, i)   (howderek_array_get(heap->store, HOWDEREK_HEAP_PARENT_INDEX(heap, i)))
#define HOWDEREK_HEAP_CHILD_INDEX(heap, i, n) ((i*heap->childrenPerNode)+n+1)
#define HOWDEREK_HEAP_CHILD(heap, i, n) (howderek_array_get(heap->store, HOWDEREK_HEAP_CHILD_INDEX(heap, i, n)))

struct howderek_heap {
    uint8_t childrenPerNode; // The d in d-ary
    struct howderek_array* store;   // The array
    // The compare function should return -1 if right is smaller than left, 0 if equal, 1 if right is larger
    int8_t (*compareFunction)(howderek_array_value_t left, howderek_array_value_t right);
};


/**
 * Create a heap
 *
 * \param size      number of elements it should hold by default.
 */
struct howderek_heap* howderek_heap_create(uint8_t childrenPerNode, int8_t (*compareFunction)(howderek_array_value_t left, howderek_array_value_t right));

/**
 * Clears the heap
 */
void howderek_heap_clear(struct howderek_heap* heap, int freeData);

/**
 * Destroys an heap
 */
void howderek_heap_destroy(struct howderek_heap* heap, int freeData);

/**
 * Adds an object to the end of an array
 */
void howderek_heap_push(struct howderek_heap* heap, howderek_array_value_t value);

/**
 * Removes the last object from the array
 */
howderek_array_value_t howderek_heap_peek(struct howderek_heap* heap);

/**
 * Removes the last object from the array
 */
howderek_array_value_t howderek_heap_pop(struct howderek_heap* heap);

#endif
