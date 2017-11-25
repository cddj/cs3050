/*! \file howderek_graph.h
    \brief Skipgraph implemenetation by Derek Howard. Good choice of data structure
           for a wide range of problems. Similar properties to BSTs and linked graphs.
            
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

#ifndef H_LIBHOWDEREK_GRAPH_H
#define H_LIBHOWDEREK_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include "howderek.h"
#include "howderek_kv.h"
#include "howderek_skiplist.h"

#define HOWDEREK_GRAPH_VALUE_PRINTF_STR "%lu"

enum howderek_graph_type {
  HOWDEREK_GRAPH_DIRECTED = 1,
  HOWDEREK_GRAPH_UNDIRECTED
};

enum howderek_vertex_status {
  HOWDEREK_GRAPH_BLACK        = 0b00000000,
  HOWDEREK_GRAPH_WHITE        = 0b00000001,
  HOWDEREK_GRAPH_GREY         = 0b00000010,
  HOWDEREK_GRAPH_RED          = 0b00000011,
  HOWDEREK_GRAPH_GREEN        = 0b00000100,
  HOWDEREK_GRAPH_BLUE         = 0b00000101,
  HOWDEREK_GRAPH_YELLOW       = 0b00000111,

  HOWDEREK_GRAPH_ALLOCATED    = 0b01000000,
  HOWDEREK_GRAPH_MARKED       = 0b10000000
};

#define HOWDEREK_GRAPH_COLOR(status)             (status & 0b00000111)
#define HOWDEREK_GRAPH_SET_COLOR(status, color)  status = ((color & 0b00000111) | (status & 0b11111000))
#define HOWDEREK_GRAPH_DATA_ALLOCATED(status)    (status & HOWDEREK_GRAPH_ALLOCATED)
#define HOWDEREK_GRAPH_MARKEDNESS(status)        (status & HOWDEREK_GRAPH_MARKED)

struct howderek_graph_edge {
  struct howderek_graph_vertex* vertex;
  struct howderek_graph_edge* next;
  double weight;                          // For A* and Bellman-Ford
};

struct howderek_graph_vertex {
  size_t id;              // Unique identified of the node. Avoid using 0
  howderek_array_value_t data;          // Data associated with this vertex
  enum howderek_vertex_status status;     // Color and markedness of the vertex
  struct howderek_graph_edge* edges;  // Edges
};

struct howderek_graph {
  struct howderek_graph_vertex* root;      // first node, where searches begin by default
  struct howderek_kv* vertex_map;     //verticies
  enum howderek_graph_type type;
  void (*onDataDisplay)(howderek_array_value_t data);
};

struct howderek_graph_pathfinding_data {
    size_t distance;
    double weightedDistance;
    struct howderek_graph_vertex* predecessor;
};

 /**
 * Create a graph
 */
struct howderek_graph* howderek_graph_create(enum howderek_graph_type type, int expectedCount);

/**
 * Destroy a graph
 * 
 * \param graph   linked graph to DESTROY!!
 */
void howderek_graph_destroy(struct howderek_graph** graphPtr, int freeData);

/**
 * Finds a path using A* between two vericies
 * 
 * \param from  starting vertex
 * \param to    ending vertex
 * \return linked graph that represents the path, NULL if path doesn't exist
 */
struct howderek_graph_search_result howderek_graph_search(
    struct howderek_graph_vertex* from,
    struct howderek_graph_vertex* to);


/**
 * Clones a graph but does not copy data.
 *
 * \param from  starting vertex
 */
struct howderek_graph* howderek_graph_clone_without_data(struct howderek_graph* graph);

/**
 * Uses Dijkstra's algorithm to create a distance graph from a given start node
 *
 * \param graph      graph to build distances from
 * \param starting   id to start from
 */
struct howderek_graph* howderek_graph_dijkstra(struct howderek_graph* graph,
                                               size_t starting);

/**
 * Uses BFS to create a distance graph from a given start node
 *
 * \param graph      graph to build distances from
 * \param starting   id to start from
 */
void howderek_graph_bfs(struct howderek_graph* graph,
                        size_t starting);

/**
 * Returns a pointer to a vertex
 *
 * \param graph  graph to search
 * \param id     id to find
 */
struct howderek_graph_vertex* howderek_graph_get(struct howderek_graph* graph,
                                                 size_t id);

/**
 * Remove a value from the graph
 * 
 * \param graph   linked graph
 * \param howderek_graph_vertex   howderek_graph_vertex to add
 */
int howderek_graph_delete(struct howderek_graph* graph, size_t id);

/**
 * Push a howderek_graph_vertex to a graph
 * 
 * \param graph   pointer to the graph to push to
 * \param id     index
 * \param data   pointer to data to store
 */
struct howderek_graph_vertex* howderek_graph_add_vertex(struct howderek_graph* graph,
                                                        size_t id, void* data);

/**
 * Adds an edge if only ids are known
 * 
 * \param graph  graph
 * \param id     index
 * \param data   pointer to data to store
 */
struct howderek_graph_edge* howderek_graph_add_edge_by_id(struct howderek_graph* graph,
                                                               size_t begin,
                                                               size_t end,
                                                               double weight);

/**
 * Push a howderek_graph_vertex to a graph
 * 
 * \param graph  graph
 * \param id     index
 * \param data   pointer to data to store
 */
struct howderek_graph_edge* howderek_graph_add_edge(struct howderek_graph* graph,
                                                    struct howderek_graph_vertex* begin,
                                                    struct howderek_graph_vertex* end,
                                                    double weight);

/**
 * Helper function to copy a struct into the heap and 
 * make sure the library knows it can free that data
 * 
 * \param vertex   vertex to add data
 * \param data   struct to copy
 * \param size   sizeof(*data)
 */
void* howderek_graph_set_data(struct howderek_graph_vertex* vertex, void* data, size_t size);

/**
 * returns a string containing info about a vertex
 * 
 * \param vertex   vertex to add data
 * \param data   struct to copy
 * \param size   sizeof(*data)
 */
char* howderek_graph_vertex_info(struct howderek_graph_vertex* vertex,
                                 void (*dataDisplayFunction)(howderek_array_value_t data));

/**
 * Load the numbers in an input file, and return a graph
 * 
 * \param input  input file
 */
struct howderek_graph* howderek_graph_import(FILE* input, enum howderek_graph_type type);

/**
 * Returns a skiplist with the addresses, useful for iteration by id
 *
 * \param graph      pointer to the graph to create a skiplist from
 * \param skiplist   skiplist to add nodes to. one will be created if NULL.
 */
struct howderek_skip* howderek_graph_index_skiplist(struct howderek_graph* graph, struct howderek_skip* skiplist);

/**
 * Load the numbers in an input file, and return a graph
 * 
 * \param graph   pointer to the graph to export
 * \param input  input file
 */
int howderek_graph_export(struct howderek_graph* graph, FILE* output);

/**
 * Draw the graph as ASCII art
 * 
 * \param graph  graph to display
 */
void howderek_graph_display(struct howderek_graph* graph);


#endif
