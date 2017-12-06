/*! \file howderek_heap.c
    \brief d-heap
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_array.h"
#include "howderek_heap.h"


int8_t __howderek_heap_default_compare_function(howderek_array_value_t left, howderek_array_value_t right) {
  return left - right;
}


struct howderek_heap* howderek_heap_create(uint8_t childrenPerNode, int8_t (*compareFunction)(howderek_array_value_t left, howderek_array_value_t right)) {
  struct howderek_heap* newHeap = malloc(sizeof(struct howderek_heap));
  newHeap->childrenPerNode = (childrenPerNode < 2) ? HOWDEREK_HEAP_DEFAULT_D : childrenPerNode;
  newHeap->store = howderek_array_create((childrenPerNode * 2) + 1);
  if (compareFunction == NULL) {
    newHeap->compareFunction = __howderek_heap_default_compare_function;
  } else {
    newHeap->compareFunction = compareFunction;
  }
  return newHeap;
}



void howderek_heap_clear(struct howderek_heap* heap, int freeData) {
  howderek_array_clear(heap->store, freeData);
}



void howderek_heap_destroy(struct howderek_heap* heap, int freeData) {
  howderek_array_destroy(heap->store, freeData);
  free(heap);
}



void howderek_heap_push(struct howderek_heap* heap, howderek_array_value_t value) {
  size_t i = heap->store->count;
  while (i != 0 && heap->compareFunction(HOWDEREK_HEAP_PARENT(heap, i), value) > 0) {
    howderek_array_swap(heap->store, i, HOWDEREK_HEAP_PARENT_INDEX(heap, i));
    i = HOWDEREK_HEAP_PARENT_INDEX(heap, i);
  }
}



howderek_array_value_t howderek_heap_pop(struct howderek_heap* heap) {
  if (heap->store->count == 0) {
    return HOWDEREK_ARRAY_EMPTY_VALUE;
  }

  howderek_array_swap(heap->store, 0, heap->store->count - 1);
  howderek_array_value_t result = howderek_array_pop(heap->store);
  size_t index = 0;
  size_t min = 0;
  size_t i = 0;
  do {
    index = min;
    for (i = 0; i < heap->childrenPerNode && HOWDEREK_HEAP_CHILD_INDEX(heap, index, i) < heap->store->count; i++) {
      if (heap->compareFunction(HOWDEREK_HEAP_CHILD(heap, index, i), howderek_array_get(heap->store, min)) == -1) {
        min = HOWDEREK_HEAP_CHILD_INDEX(heap, index, i);
      }
    }
    if (min != index) {
      howderek_array_swap(heap->store, index, min);
    }
  } while (index != min);
 return result;
}
