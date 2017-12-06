/*! \file howderek_array.h
    \brief Key-value store. Uses either a dynamic array or hashmap depending on how sparse the data is.
*/

#ifndef H_LIBHOWDEREK_KV_H
#define H_LIBHOWDEREK_KV_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_array.h"
#include "howderek_hashmap.h"

#define HOWDEREK_KV_MIN_TO_SWITCH 3

enum howderek_kv_containers {
  HOWDEREK_KV_ARRAY   = 1,
  HOWDEREK_KV_HASHMAP = 2
};


struct howderek_kv {
    struct howderek_array* array;
    struct howderek_hashmap* hashmap;
    size_t count;
    size_t size;
    size_t expectedSize;
    float zeroRatio;
    float threshold;
    enum howderek_kv_containers type;
};


struct howderek_kv_iter {
    struct howderek_hashmap_iter  hashmap_iter;
    howderek_array_value_t*       array_iter;
    size_t                        key;
    howderek_array_value_t        value;
};

/**
 * Create a key-value store
 *
 * \param size      number of elements it should hold by default.
 */
struct howderek_kv* howderek_kv_create(size_t size, enum howderek_kv_containers type);


/**
 * Retrieve an object from the array
 *
 * \param kv     the howderek_kv to retrieve from
 * \param index  the index to retrieve
 */
howderek_array_value_t howderek_kv_get(struct howderek_kv* kv, size_t key);

/**
 * Sets the value for a key
 *
 * \param kv     the howderek_kv to retrieve from
 * \param key    key of the object to set
 * \param value  value to set the key to
 */
void howderek_kv_set(struct howderek_kv* kv, size_t key, howderek_array_value_t value);

/**
 * Removes an object from the array
 *
 * \param kv     the howderek_kv to delete from
 * \param key    key of the object to delete
 */
howderek_array_value_t howderek_kv_delete(struct howderek_kv* kv, size_t key);

/**
 * Fills a howderek_kv_iter with the information needed to iterate
 *
 * \param kv      the howderek_kv to iterate on
 * \param iter    pointer to the howderek_kv_iter to fill
 */
void howderek_kv_fill_iter(struct howderek_kv* kv, struct howderek_kv_iter* iter);

/**
 * Iterates over a howderek_kv using a howderek_kv_iter
 *
 * \param iter  howderek_kv_iter generated from howderek_array_kv_set_iter
 */
int howderek_kv_iterate(struct howderek_kv* kv, struct howderek_kv_iter* iter);

/**
 * Destroys a howderek_kv
 *
 * \param kv     the howderek_kv to destroy
 */
void howderek_kv_destroy(struct howderek_kv* kv, int freeData);

#endif
