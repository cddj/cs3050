/*! \file howderek_graphgraph.h
    \brief Implementation of a graph
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_kv.h"
#include "howderek_skiplist.h"
#include "howderek_heap.h"
#include "howderek_graph.h"

 /**
 * Create a graph
 *
 * \param compareFunction   Function that returns a negative value if from is less than
                            to, 0 if equal, positive if from is greater than to.
                            If NULL, will create with default compare function (compare keys)
   \param mutationFunction  Function that runs if an key alrady exists during a push. Used
                            to modify the data in the graph. If NULL, will create with
                            default mutation function (replace oldData with newData)
 */

struct howderek_graph* howderek_graph_create(enum howderek_graph_type type, int expectedCount) {
  expectedCount = (expectedCount >= 0) ? expectedCount : 0;
  struct howderek_graph* graph =  malloc(sizeof(struct howderek_graph));
  graph->vertex_map = howderek_kv_create(expectedCount, HOWDEREK_KV_ARRAY);
  graph->root = NULL;
  graph->type = type;
  return graph;
}



struct howderek_graph_vertex* howderek_graph_get(struct howderek_graph* graph, size_t key) {
  return howderek_kv_get(graph->vertex_map, key);
}



void __howderek_graph_destroy_vertex(void* vertex_as_void_ptr) {
  struct howderek_graph_vertex * vertex = vertex_as_void_ptr;
  struct howderek_graph_edge* iter;
  struct howderek_graph_edge* iter2;
  struct howderek_graph_edge* the_dead; // Edges to DESTROY!


  iter = vertex->edges;
  while (iter != NULL) {
    // Don't let other verticies have an edge from vertex
    the_dead = iter;
    iter = iter->next;
    free(the_dead);
  }

  free(vertex_as_void_ptr);
}



int howderek_graph_delete(struct howderek_graph* graph, size_t key) {
  struct howderek_graph_vertex* vertex = howderek_graph_get(graph, key);
  if (vertex == NULL) {
    return 0;
  } else {
    if (vertex == graph->root) {
      graph->root = vertex->edges->vertex;
    }
    __howderek_graph_destroy_vertex(vertex);
    howderek_kv_delete(graph->vertex_map, key);
    return 1;
  }
}



void howderek_graph_destroy(struct howderek_graph** graphPtr, int freeData) {
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter((*graphPtr)->vertex_map, &iter);
  while (howderek_kv_iterate((*graphPtr)->vertex_map, &iter)) {
    struct howderek_graph_vertex* vertex = iter.value;
    if (freeData) {
      free(vertex->data);
    }
    __howderek_graph_destroy_vertex(vertex);
  }
  howderek_kv_destroy((*graphPtr)->vertex_map, 0);
  free(*graphPtr);
  *graphPtr = NULL;
}

/**
 * Push a howderek_graph_vertex to a graph
 *
 * \param graph   pointer to the graph to push to
 * \param key     index
 * \param data   pointer to data to store
 */
struct howderek_graph_vertex* howderek_graph_add_vertex(struct howderek_graph* graph, size_t key, void* data) {
  struct howderek_graph_vertex* vertex = malloc(sizeof(struct howderek_graph_vertex));
  vertex->id = key;
  vertex->data = data;
  vertex->status = 0;
  vertex->edges = NULL;
  if (graph->root == NULL) {
    graph->root = vertex;
  }
  howderek_kv_set(graph->vertex_map, key, vertex);
  return vertex;
}



struct howderek_graph_edge* howderek_graph_add_edge_by_id(struct howderek_graph* graph, size_t begin,
                                                   size_t end, double weight) {
  struct howderek_graph_vertex* from = howderek_graph_get(graph, begin);
  struct howderek_graph_vertex* to = howderek_graph_get(graph, end);
  return howderek_graph_add_edge(graph, from, to, weight);
}



struct howderek_graph_edge* howderek_graph_add_edge(struct howderek_graph* graph, struct howderek_graph_vertex* begin, struct howderek_graph_vertex* end, double weight) {
  if (begin == NULL || end == NULL) {
    return NULL;
  }

  struct howderek_graph_edge* edge = malloc(sizeof(struct howderek_graph_edge)); // doing it this way will make freeing easier

  struct howderek_graph_edge** iter;
  iter = &(begin->edges);
  while (*iter != NULL) {
      iter = &((*iter)->next);
  }
  *iter   = edge;
  edge->vertex = end;
  edge->weight = weight;
  edge->next   = NULL;

  if (graph != NULL && graph->type == HOWDEREK_GRAPH_UNDIRECTED) {
    // Add the reverse edge if this is undirected.
    howderek_graph_add_edge(NULL, end, begin, weight);
  }

  return begin->edges;
}


struct howderek_graph* howderek_graph_clone_without_data(struct howderek_graph* graph) {
  if (graph == NULL) {
    return howderek_graph_create(HOWDEREK_GRAPH_UNDIRECTED, 0);
  }
  struct howderek_graph* newGraph = howderek_graph_create(HOWDEREK_GRAPH_UNDIRECTED, graph->vertex_map->count);
  howderek_graph_add_vertex(newGraph, graph->root->id, NULL);
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter(graph->vertex_map, &iter);
  while (howderek_kv_iterate(graph->vertex_map, &iter)) {
    if (iter.key != graph->root->id) {
      howderek_graph_add_vertex(newGraph, iter.key, NULL);
    }
  }
  howderek_kv_fill_iter(graph->vertex_map, &iter);
  while (howderek_kv_iterate(graph->vertex_map, &iter)) {
    struct howderek_graph_vertex* vertex = ((struct howderek_graph_vertex*)(iter.value));
    struct howderek_graph_edge* edge = vertex->edges;
    while (edge != NULL) {
      howderek_graph_add_edge_by_id(newGraph, iter.key, edge->vertex->id, edge->weight);
      edge = edge->next;
    }
  }
  newGraph->root = howderek_graph_get(newGraph, graph->root->id);
  return newGraph;
}



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


int8_t __howderek_graph_weighted_distance_compare(howderek_array_value_t left, howderek_array_value_t right) {
  struct howderek_graph_vertex* u = left;
  struct howderek_graph_vertex* v = right;

  if (u != NULL &&
      v != NULL &&
      u->id != HOWDEREK_HASHMAP_NULL_ID &&
      v->id != HOWDEREK_HASHMAP_NULL_ID &&
      u->data != NULL &&
      v->data != NULL) {

    struct howderek_graph_pathfinding_data* uData = u->data;
    struct howderek_graph_pathfinding_data* vData = v->data;

    if (u->id == v->id) {
      return 0;
    } else {
      int8_t result = (uData->weightedDistance == vData->weightedDistance) ? 0 : ((uData->weightedDistance > vData->weightedDistance) ? 1 : -1);
      return result;
    }

  } else if (u != NULL && v != NULL) {
    return u->id - v->id;
  } else {
    return -1;
  }
}

void __howderek_weight_display(howderek_array_value_t distances_as_void_ptr) {
  struct howderek_graph_pathfinding_data* distances = distances_as_void_ptr;
  if (isfinite(distances->weightedDistance)) {
    printf("weighted distance:   %lf\n", distances->weightedDistance);
    printf("unweighted distance: %lu\n", distances->distance);
    printf("shortest path:       this");
    struct howderek_graph_vertex* iter = distances->predecessor;
    while (iter != NULL) {
      printf(" <- %lu", iter->id);
      distances = iter->data;
      iter = distances->predecessor;
    }
    printf("\n");
  } else {
    printf("unreachable\n");
  }
}

int8_t __howderek_graph_weighted_compare(howderek_array_value_t left, howderek_array_value_t right) {
  if (((struct howderek_graph_vertex*) left)->data == NULL
      || ((struct howderek_graph_vertex*) right)->data == NULL) {
    return 1;
  }
  struct howderek_graph_pathfinding_data* l =((struct howderek_graph_vertex*) left)->data;
  struct howderek_graph_pathfinding_data* r =((struct howderek_graph_vertex*) right)->data;
  if (l == NULL) {
    return -1;
  }
  if (r == NULL) {
    return 1;
  }
  int8_t result = (l->weightedDistance == r->weightedDistance) ? 0 : ((l->weightedDistance > r->weightedDistance) ? 1 : -1);
  return result;
}

struct howderek_graph* howderek_graph_dijkstra(struct howderek_graph* graph, size_t starting) {
  struct howderek_graph* distanceGraph = howderek_graph_clone_without_data(graph);
  distanceGraph->onDataDisplay = __howderek_weight_display;
  struct howderek_heap* queue = howderek_heap_create(0, __howderek_graph_weighted_distance_compare);
  struct howderek_graph_pathfinding_data* dataPool =  malloc(sizeof(struct howderek_graph_pathfinding_data)
                                                             * distanceGraph->vertex_map->count);
  size_t k = 0;
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter(graph->vertex_map, &iter);

  while (howderek_kv_iterate(distanceGraph->vertex_map, &iter)) {
    dataPool[k].distance = HOWDEREK_HASHMAP_INFINITY;
    dataPool[k].weightedDistance = INFINITY;
    dataPool[k].predecessor = NULL;
    struct howderek_graph_vertex* currentVertex = iter.value;
    currentVertex->data = &dataPool[k];
    howderek_heap_push(queue, currentVertex);
    k++;
  }
  if (starting == 0) {
     starting = graph->root->id;
  }
  struct howderek_graph_vertex* parentVertex = howderek_graph_get(distanceGraph, starting);
  const enum howderek_vertex_status unvisitedColor = HOWDEREK_GRAPH_COLOR(parentVertex->status);
  const enum howderek_vertex_status visitedColor = (unvisitedColor == HOWDEREK_GRAPH_BLACK) ? HOWDEREK_GRAPH_WHITE : HOWDEREK_GRAPH_BLACK;
  HOWDEREK_GRAPH_SET_COLOR(parentVertex->status, visitedColor);
  // Set the distance of the first node to 0
  struct howderek_graph_pathfinding_data* parentDistance = parentVertex->data;
  parentDistance->distance = 0;
  parentDistance->weightedDistance = 0.0f;
  // Make sure it will be in the front of the queue
  howderek_heap_push(queue, parentVertex);
  struct howderek_graph_vertex* currentVertex;
  while ((currentVertex = howderek_heap_pop(queue))) {
    if (HOWDEREK_GRAPH_COLOR(currentVertex->status) != visitedColor) {
      struct howderek_graph_edge* iter = currentVertex->edges;
      while (iter != NULL) {
        struct howderek_graph_pathfinding_data* storedDistances = iter->vertex->data;
        double distance = ((struct howderek_graph_pathfinding_data*)currentVertex->data)->weightedDistance + iter->weight;
        if (distance < storedDistances->weightedDistance) {
          storedDistances->predecessor = currentVertex;
          storedDistances->weightedDistance = distance;
          storedDistances->distance = ceil(distance);
          howderek_heap_push(queue, iter->vertex);
        }
        iter = iter->next;
      }
      parentVertex = currentVertex;
    }
  }
  return distanceGraph;
}

struct howderek_graph_pathfinding_data* __howderek_graph_create_distance(double weighted,
                                                          size_t unweighted,
                                                          struct howderek_graph_vertex* pred) {
  struct howderek_graph_pathfinding_data* distanceData = malloc(sizeof(struct howderek_graph_pathfinding_data));
  distanceData->distance = unweighted;
  distanceData->weightedDistance = weighted;
  distanceData->predecessor = pred;
  return distanceData;
}


void howderek_graph_bfs(struct howderek_graph* graph,
                        size_t starting) {
  struct howderek_graph_vertex* startingVertex = howderek_graph_get(graph, starting);
  if (startingVertex == NULL) {
    startingVertex = graph->root;
  }
  startingVertex->data = __howderek_graph_create_distance(0.0, 0, NULL);
  struct howderek_array* queue = howderek_array_create(1);
  const enum howderek_vertex_status unvisitedColor = HOWDEREK_GRAPH_COLOR(startingVertex->status);
  const enum howderek_vertex_status visitedColor = (unvisitedColor == HOWDEREK_GRAPH_BLACK) ? HOWDEREK_GRAPH_WHITE : HOWDEREK_GRAPH_BLACK;
  howderek_array_push(queue, startingVertex);
  while (queue->count > 0) {
    struct howderek_graph_vertex* parent = howderek_array_dequeue(queue);
    if (parent == NULL || HOWDEREK_GRAPH_COLOR(parent->status) == visitedColor) {
      continue;
    }
    struct howderek_graph_edge* iter = parent->edges;
    while (iter != NULL) {
      size_t potentialDistance = ((struct howderek_graph_pathfinding_data*)(parent->data))->distance + 1;
      if ((iter->vertex->data != NULL
          && potentialDistance < ((struct howderek_graph_pathfinding_data*)(iter->vertex->data))->distance)
          || iter->vertex->data == NULL) {
        if (iter->vertex->data == NULL) {
          iter->vertex->data = __howderek_graph_create_distance(0.0, potentialDistance, parent);
        } else {
          ((struct howderek_graph_pathfinding_data*)(iter->vertex->data))->distance = potentialDistance;
          ((struct howderek_graph_pathfinding_data*)(iter->vertex->data))->predecessor = parent;
        }
      }
      if (HOWDEREK_GRAPH_COLOR(iter->vertex->status) == unvisitedColor) {
        HOWDEREK_GRAPH_SET_COLOR(iter->vertex->status, HOWDEREK_GRAPH_GREY);
        howderek_array_push(queue, iter->vertex);
      }
      iter = iter->next;
    }
    HOWDEREK_GRAPH_SET_COLOR(parent->status, visitedColor);
  }
}



const char* howderek_graph_color_string(struct howderek_graph_vertex* vertex) {
  const char* colors[18] = {
    HOWDEREK_INVERSE HOWDEREK_BOLD "black" HOWDEREK_RESET,
    "white",
    HOWDEREK_INVERSE "grey" HOWDEREK_RESET,
    HOWDEREK_RED "red" HOWDEREK_RESET,
    HOWDEREK_GREEN "green" HOWDEREK_RESET,
    HOWDEREK_BLUE "blue" HOWDEREK_RESET,
    HOWDEREK_YELLOW "yellow" HOWDEREK_RESET
  };
  return colors[HOWDEREK_GRAPH_COLOR(vertex->status)];
}



void howderek_graph_display_vertex_info(struct howderek_graph_vertex* vertex,
                                        void (*dataDisplayFunction)(howderek_array_value_t data)) {

  printf("Vertex %lu"
         "\n---------------------\n"
         HOWDEREK_BOLD "color" HOWDEREK_RESET ": %s\n"
         HOWDEREK_BOLD "outgoing edges" HOWDEREK_RESET ":\n", vertex->id, howderek_graph_color_string(vertex));

  struct howderek_graph_edge* iter = vertex->edges;
  while (iter != NULL) {
    printf("this                 -> %-20lu (weight: %f)\n", iter->vertex->id, iter->weight);
    iter = iter->next;
  }
  printf(HOWDEREK_BOLD "incoming edges" HOWDEREK_RESET ":\n");
  if (dataDisplayFunction != NULL && vertex->data != NULL) {
    printf(HOWDEREK_BOLD "data" HOWDEREK_RESET ":\n");
    dataDisplayFunction(vertex->data);
  }
  printf("\n");
}



struct howderek_graph* howderek_graph_import(FILE* input, enum howderek_graph_type type);



struct howderek_skip* howderek_graph_index_skiplist(struct howderek_graph* graph, struct howderek_skip* skiplist) {
  skiplist = (skiplist == NULL) ? howderek_skip_create() : skiplist;
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter(graph->vertex_map, &iter);
  while (howderek_kv_iterate(graph->vertex_map, &iter)) {
    howderek_skip_push(skiplist, iter.key, iter.value);
  }
  return skiplist;
}



int howderek_graph_export(struct howderek_graph* graph, FILE* output);



void howderek_graph_display(struct howderek_graph* graph) {
  printf("Graph Type: %s\n\nInternal kv ", (graph->type == HOWDEREK_GRAPH_UNDIRECTED) ? "Undirected" : "Directed");
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter(graph->vertex_map, &iter);
  while (howderek_kv_iterate(graph->vertex_map, &iter)) {
    howderek_graph_display_vertex_info(iter.value, NULL);
  }
}
