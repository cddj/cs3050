/*! \file howderek_skiplist.h
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

#ifndef H_LIBHOWDEREK_SKIPLIST_H
#define H_LIBHOWDEREK_SKIPLIST_H

#include <stdio.h>
#include <stdlib.h>

#ifndef HOWDEREK_SKIP_MAX_LEVEL
#define HOWDEREK_SKIP_MAX_LEVEL 20
#endif

typedef long int howderek_skip_value_t;
#define HOWDEREK_SKIP_VALUE_PRINTF_STR "%lu"

enum howderek_skip_data_status {
  HOWDEREK_SKIP_DATA_ON_STACK = 1,
  HOWDEREK_SKIP_DATA_IN_HEAP
};

struct howderek_skip_node {
  howderek_skip_value_t id;
  int level;
  void* data; // Data associated with this node
  struct howderek_skip_node** next; // All linked nodes
};

struct howderek_skip {
  struct howderek_skip_node* head;
  howderek_skip_value_t (*compareFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right); // Function that is used to compare nodes and sort
  void (*mutationFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right); // Function that is used when an id already exists
  size_t count;
  size_t ops;
  int level;
};

 /**
 * Create a skip list
 * 
 * \param compareFunction   Function that returns a negative value if left is less than
                            right, 0 if equal, positive if left is greater than right.
                            If NULL, will create with default compare function (compare IDs)
   \param mutationFunction  Function that runs if an ID alrady exists during a push. Used
                            to modify the data in the skip list. If NULL, will create with
                            default mutation function (replace oldData with newData)
 */
struct howderek_skip* howderek_skip_create_custom(howderek_skip_value_t (*compareFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right), void (*mutationFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right));

/**
 * Create a skip list that sorts by ID and replaces on conflicts (default behavior)
 */
struct howderek_skip* howderek_skip_create(void);

/**
 * Replaces the comparison function. Rebuilds the skip list using the new comparison.
 */
void howderek_skip_set_compare_function(struct howderek_skip** list, howderek_skip_value_t (*compareFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right));

/**
 * Replaces the mutation function
 * 
 * \param list               pointer to the pointer to the skip list to rebuild
 * \param mutationFunction   new mutatuon function
 */
void howderek_skip_set_mutation_function(struct howderek_skip** list, void (*mutationFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right));

/**
 * Destroy a skiplist
 * 
 * \param list   linked list to DESTROY!!
 */
void howderek_skip_destroy(struct howderek_skip* list, int freeData);

/**
 * Return a pointer to a value in the skiplist or NULL if not found
 * 
 * \param input  input file
 */
struct howderek_skip_node* howderek_skip_search(struct howderek_skip* list, howderek_skip_value_t id, void* data);

/**
 * Returns the ith element
 *
 * \param input  input file
 */
struct howderek_skip_node* howderek_skip_index(struct howderek_skip* list, size_t i);

/**
 * Returns the ith element
 *
 * \param input  input file
 */
int howderek_skip_delete_index(struct howderek_skip* list, size_t i, int freeData);

/**
 * Remove a value from the skiplist
 * 
 * \param list   linked list
 * \param howderek_skip_node   howderek_skip_node to add
 */
int howderek_skip_delete(struct howderek_skip* list, howderek_skip_value_t id, void* data);

/**
 * Push a howderek_skip_node to a skip list
 * 
 * \param list   pointer to the skip list to push to
 * \param id     index
 * \param data   pointer to data to store
 */
void howderek_skip_push(struct howderek_skip* list, howderek_skip_value_t id, void* data);

/**
 * Reinsert a value into the skiplist after it has been modified
 *
 * \param list   pointer to the skip list to push to
 * \param id     index
 * \param data   pointer to data to store
 */
void howderek_skip_update(struct howderek_skip* list, howderek_skip_value_t id, void* oldData, void* newData);

/**
 * Helper function to copy a struct into the heap and 
 * make sure the library knows it can free that data
 * 
 * \param node   node to add data
 * \param data   struct to copy
 * \param size   sizeof(*data)
 */
void* howderek_skip_set_data(struct howderek_skip_node* node, void* data, size_t size);

/**
 * Load the numbers in an input file, and return a skip list
 * 
 * \param input  input file
 */
struct howderek_skip* howderek_skip_import(FILE* input);

/**
 * Load the numbers in an input file, and return a skip list
 * 
 * \param list   pointer to the skip list to export
 * \param input  input file
 */
int howderek_skip_export(struct howderek_skip* list, FILE* input);

/**
 * Draw the skip list as ASCII art
 * 
 * \param list  skip list to display
 */
void howderek_skip_display(struct howderek_skip* list);

#endif
