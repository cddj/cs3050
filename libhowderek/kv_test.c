/*! \file generate_test_file.c
 *  \author Derek Howard
 *  \date 17 October 2017
 *  \brief Gerenates a test file for HW3
 *  \copyright Copyright (c) 2017 Derek Howard
 *             Distributed under the CC0 license
 *             <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
*/
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "howderek.h"
#include "howderek_heap.h"

struct testType {
    char text[16];
    int64_t priority;
};

int8_t priority_compare(howderek_array_value_t left, howderek_array_value_t right) {
  struct testType* l = left;
  struct testType* r = right;
  if (l == NULL) {
    return -1;
  }
  if (r == NULL) {
    return 1;
  }
  int8_t result = (l->priority == r->priority) ? 0 : ((l->priority > r->priority) ? 1 : -1);
  return result;
}

void display(struct howderek_array* array, size_t index, howderek_array_value_t* data) {
  struct testType* obj = *data;
  printf("%lu: { %s, %lld }\n", index, obj->text, obj->priority);
}


int main(int argc, char** argv) {
  howderek_set_log_level(HOWDEREK_LOG_DEBUG);
  howderek_log(HOWDEREK_LOG_DEBUG, "creating heap...");
  struct howderek_heap* heap = howderek_heap_create(4, &priority_compare);
  howderek_log(HOWDEREK_LOG_DEBUG, "adding elements...");
  struct testType largest = {"largest", 9734242};
  struct testType large = {"large", 424242};
  struct testType medium = {"medium", 12345};
  struct testType small = {"small", 42};
  struct testType smaller = {"smaller", 10};
  struct testType smallest = {"smallest", -42};
  howderek_heap_push(heap, &smaller);
  howderek_heap_push(heap, &medium);
  howderek_heap_push(heap, &largest);
  howderek_heap_push(heap, &smallest);
  howderek_heap_push(heap, &large);
  howderek_heap_push(heap, &small);
  struct testType* iter;
  howderek_log(HOWDEREK_LOG_DEBUG, "heap size: %lu", heap->store->count);
  while ((iter = howderek_heap_pop(heap))) {
    printf("popped %s \n", iter->text);
    howderek_array_for_each(heap->store, display);
  }
  printf("\n");
}
