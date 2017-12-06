/*! \file howderek_hashmaplist.h
    \brief Skiplist implemenetation by Derek Howard. Good choice of data structure
           for a wide range of problems. Similar properties to BSTs and linked lists.
            
                       Average    Worst-case
           
           Space       O(n)       O(n log n)
           Search      O(log n)   O(n)
           Insert      O(log n)   O(n)
           Delete      O(log n)   O(n)

           Thanks to William Pugh [1] for his description of Skip Lists and Thomas 
           Niemann at the University of Auckland [2] for his reference implementation 
           and visual explanations.

           [1] http://delivery.acm.org/10.1145/80000/78977/p668-pugh.pdf
           [2] https://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_skl.htm
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"

typedef uint64_t howderek_hashmap_key_t;
typedef void* howderek_hashmap_value_t;

#pragma once

#define HOWDEREK_HASHMAP_INFINITY UINT64_MAX;
#define HOWDEREK_HASHMAP_ALLOCATED_VALUES 1
#define HOWDEREK_HASHMAP_KEY_PRINTF_STR "llu"
#define HOWDEREK_HASHMAP_VALUE_PRINTF_STR "p"
#define HOWDEREK_HASHMAP_HASH_FUNCTIONS 3
#define HOWDEREK_HASHMAP_THRESHOLD 0.90
#define HOWDEREK_HASHMAP_NULL_ID 0
#define HOWDEREK_HASHMAP_MIN_SIZE 0


struct howderek_hashmap_node {
  howderek_hashmap_key_t   key;
  howderek_hashmap_value_t value;
  /* flags: UUUU UUUA
   * U: undefined
   * A: 0 static, 1 allocated
   */
  uint8_t flags;
};

// Bit Masks
#define HOWDEREK_HASHMAP_NODE_ALLOCATED(flags)  (flags & 0b00000001)

struct howderek_hashmap {
  size_t count;
  size_t size;
  int    __primeCount;
  struct howderek_hashmap_node* tables[HOWDEREK_HASHMAP_HASH_FUNCTIONS];
};

struct howderek_hashmap_iter {
    struct howderek_hashmap_node* node;
    struct howderek_hashmap_iter* next;
};

/**
 * Create a hashmap
 */
struct howderek_hashmap* howderek_hashmap_create(size_t size);

/**
 * Reset a hashmap to its initial state
 */
void howderek_hashmap_clear(struct howderek_hashmap** hashmap_ptr);

/**
 * Add a value to the hashmap
 * 
 * \param hashmap    hashmap to add to
 * \param key        key    (default type: uint64_t)
 * \param value      value  (default type: void*)
 * \param allocated  1 if value malloc'd, 0 if not
 * \return hashmap
 */
struct howderek_hashmap_node* howderek_hashmap_add_with_flags(struct howderek_hashmap** hashmap_ptr,
                                                   howderek_hashmap_key_t key,
                                                   howderek_hashmap_value_t value,
                                                   uint8_t flags);


/**
 * Adds a static value to the hashmap. Will not free the value when removed.
 *
 * \param hashmap    hashmap to add to
 * \param key        key    (default type: uint64_t)
 * \param value      value  (default type: void*)
*/
struct howderek_hashmap_node* howderek_hashmap_set_static(struct howderek_hashmap** hashmap_ptr,
                                                          howderek_hashmap_key_t key,
                                                          howderek_hashmap_value_t value);

/**
 * Add a value to the hashmap. Assumes the value was dynamically allocated.
 *
 * \param hashmap    hashmap to add to
 * \param key        key    (default type: uint64_t)
 * \param value      value  (default type: void*)
*/
struct howderek_hashmap_node* howderek_hashmap_set(struct howderek_hashmap** hashmap_ptr,
                                                          howderek_hashmap_key_t key,
                                                          howderek_hashmap_value_t value);
/**
 * Return a pointer to a value in the skiplist or NULL if not found
 *
 * \param hashmap  hashmap to remove from
 * \param key      key with value to remove    (default type: uint64_t)
 * \return         1 if successful, 0 otherwise
 */
int howderek_hashmap_remove(struct howderek_hashmap** hashmap_ptr,
                                                 howderek_hashmap_key_t key);


/**
 * Return the slot that matches a key
 *
 * \param hashmap  hashmap to search
 * \param key      key    (default type: uint64_t)
 * \return         the value if found, 0 if not
 */
struct howderek_hashmap_node* howderek_hashmap_hash(struct howderek_hashmap* hashmap, howderek_hashmap_key_t key);

/**
 * Return a pointer to a value in the skiplist or NULL if not found
 * 
 * \param hashmap  hashmap to search
 * \param key      key    (default type: uint64_t)
 * \return         the value if found, 0 if not
 */
howderek_hashmap_value_t howderek_hashmap_get(struct howderek_hashmap** hashmap_ptr,
                                              howderek_hashmap_key_t key);

/**
 * Run a function over each member
 *
 * \param hashmap  hashmap to iterate on
 * \param func     function that accepts a pointer to a node
 */
void howderek_hashmap_for_each(struct howderek_hashmap** hashmap_ptr,
                               void (*func)(struct howderek_hashmap_node* node));


/**
 * Run a function over each member sequentially. Slower than howderek_hashmap_for_each.
 *
 * \param hashmap  hashmap to iterate on
 * \param func     function that accepts a pointer to a node
 */
void howderek_hashmap_for_each_sequential(struct howderek_hashmap** hashmap_ptr,
                                          void (*func)(struct howderek_hashmap_node* node));

/**
 * Run a function over each data
 *
 * \param hashmap  hashmap to iterate on
 * \param func     function that accepts a pointer to a node
 */
void howderek_hashmap_for_each_value(struct howderek_hashmap** hashmap_ptr,
                                     void (*func)(howderek_hashmap_value_t));

/**
 * Run a function over each member's data sequentially.
 * Slower than howderek_hashmap_for_each.
 *
 * \param hashmap  hashmap to iterate on
 * \param func     function that accepts a pointer to a node
 */
void howderek_hashmap_for_each_value_sequential(struct howderek_hashmap** hashmap_ptr,
                                                void (*func)(howderek_hashmap_value_t));

/**
 * Create iterator list
 *
 * \param hashmap  hashmap to iterate on
 * \param iter     iterator to set
 * \return         howderek_hashmap_iter
 */
void howderek_hashmap_fill_iter(struct howderek_hashmap** hashmap_ptr,
                                struct howderek_hashmap_iter* hashmap_iter);

/**
 * Iterate over the hashmap iterator
 *
 * \param hashmap  hashmap to iterate on
 * \param iter     iterator to set
 * \return         1 if values still remain, 0 otherwise
 */
int howderek_hashmap_iterate(struct howderek_hashmap_iter* hashmap_iter);

/**
 * Frees the memory used by a hashmap
 *
 * \param hashmap  hashmap to destroy
 */
void howderek_hashmap_destroy(struct howderek_hashmap** hashmap_ptr, uint8_t freeData);

/**
 * Load the numbers in an input file, and return a skip list
 * 
 * \param input  input file
 */
struct howderek_hashmap* howderek_hashmap_import(FILE* input);

/**
 * Load the numbers in an input file, and return a skip list
 * 
 * \param list   pointer to the skip list to export
 * \param input  input file
 */
int howderek_hashmap_export(struct howderek_hashmap** hashmap_ptr, FILE* output);

/**
 * Draw the hashmap as ASCII art
 * 
 * \param hashmap  hashmap to display
 */
void howderek_hashmap_display(struct howderek_hashmap** hashmap_ptr, uint8_t showTables);
