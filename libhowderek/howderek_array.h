/*! \file howderek_array.c
 *  \brief Dynamic circular array
 */

#ifndef H_LIBHOWDEREK_ARRAY_H
#define H_LIBHOWDEREK_ARRAY_H

#ifndef HOWDEREK_ARRAY_CONFIG
#define HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS 3
#define HOWDEREK_MEMORY_HASHMAP_DEFAULT_SIZE 5
typedef void* howderek_array_value_t;
#define HOWDEREK_ARRAY_EMPTY_VALUE NULL
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"
#include "howderek_memory.h"

struct howderek_array {
    howderek_array_value_t* array;   // The actual array
    size_t size;                     // How much it can hold
    size_t count;                    // How many objects are in the array
    size_t zero;  // For circular buffer
    size_t end;   // For pop
};


/**
 * Create a dynamic array
 *
 * \param size      number of elements it should hold by default.
 */
struct howderek_array* howderek_array_create(size_t size);


/**
 * Increase the size of an array
 *
 * \param size      number of elements it should hold by default.
 */
void howderek_array_grow(struct howderek_array* array, size_t size);

/**
 * Retrieve an object's address in the array
 *
 * \param size      number of elements it should hold by default.
 */
howderek_array_value_t* howderek_array_at(struct howderek_array* array,
                                          size_t index);

/**
 * Retrieve an object from the array
 *
 * \param size      number of elements it should hold by default.
 */
howderek_array_value_t howderek_array_get(struct howderek_array* array,
                                          size_t index);

/**
 * Removes an object from the array
 *
 * \param size      number of elements it should hold by default.
 */
howderek_array_value_t* howderek_array_set(struct howderek_array* array,
                                           size_t index,
                                           howderek_array_value_t value);


/**
 * Swaps two values in the array
 *
 * \param size      number of elements it should hold by default.
 */
void howderek_array_swap(struct howderek_array* array, size_t left,
                         size_t right);

/**
 * Removes an object from the array
 */
howderek_array_value_t howderek_array_remove(struct howderek_array* array,
                                             size_t index);

/**
 * Clears the array
 */
void howderek_array_clear(struct howderek_array* array, int freeData);

/**
 * Destroys an array
 */
void howderek_array_destroy(struct howderek_array* array, int freeData);

/**
 * Adds an object to the end of an array
 */
howderek_array_value_t* howderek_array_push(struct howderek_array* array,
                                            howderek_array_value_t value);


/**
 * Removes the last object from the array
 */
howderek_array_value_t howderek_array_pop(struct howderek_array* array);

/**
 * Adds an object to a position and if there is already something there, move
 * it to the right
 */
howderek_array_value_t* howderek_array_insert(struct howderek_array* array,
                                              size_t position,
                                              howderek_array_value_t value);

/**
 * Removes the first object from the array
 */
howderek_array_value_t howderek_array_dequeue(struct howderek_array* array);

/**
 * Moves a range to another range
 */
howderek_array_value_t* howderek_array_slide(struct howderek_array* array,
                                             size_t fromBegin, size_t fromEnd,
                                             size_t to);

/**
 * Iterates over a howderek_array array. Returns 1 if value, 0 otherwise
 */
int howderek_array_iterate(struct howderek_array* array,
                           howderek_array_value_t** iter);

/**
 * Runs a function on each value of the array
 */
void howderek_array_for_each(struct howderek_array* array,
                            void(*func)(struct howderek_array* array,
                                        size_t index,
                                        howderek_array_value_t* value));

#endif
