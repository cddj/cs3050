/*! \file howderek_array.c
 *  \brief Dynamic circular array
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_array.h"

#ifndef _SC_PAGESIZE
#define _SC_PAGESIZE 4096
#endif

struct howderek_array* howderek_array_create(size_t size) {
  struct howderek_array* newArray;
  newArray = malloc(sizeof(struct howderek_array));
  newArray->count = 0;
  newArray->array = NULL;
  howderek_array_grow(newArray, size);
  return newArray;
}



void howderek_array_grow(struct howderek_array* array, size_t size) {
  if (array == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR,
                 "libhowderek_array: howderek_array_grow called on array=NULL");
  }
  struct howderek_array newArray;
  // Only deal with full pages
  const size_t mallocsize = (
        ((size * sizeof(howderek_array_value_t)) / sysconf(_SC_PAGESIZE))
        * sysconf(_SC_PAGESIZE))
      + sysconf(_SC_PAGESIZE);
  const size_t potentialSize = mallocsize / sizeof(howderek_array_value_t);
  newArray.array = malloc(mallocsize);
  if (newArray.array == NULL) {
    howderek_log(HOWDEREK_LOG_FATAL,
                 "libhowderek_array: resize failed for size %lu (%lu bytes)",
                 size, mallocsize);
    return;
  }
  newArray.size = potentialSize;
  newArray.zero = 0;
  newArray.end = array->end;
  newArray.count = array->count;
  size_t i;
  // Copy old elements
  if (array != NULL) {
    for (i = 0; i < array->count && i < newArray.size; i++) {
      newArray.array[i] = howderek_array_get(array, i);
    }
    for (; i < newArray.size; i++) {
      newArray.array[i] = HOWDEREK_ARRAY_EMPTY_VALUE;
    }
    if (array->array != NULL) {
      free(array->array);
      array->array = NULL;
    }
  }
  array->array = newArray.array;
  array->size  = newArray.size;
  array->zero  = 0;
}



howderek_array_value_t* howderek_array_at(struct howderek_array* array, size_t index) {
  if (array == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR,
                 "libhowderek_array: howderek_array_at called on array=NULL");
    return NULL;
  }
  if (array->zero + index >= array->size) {
    size_t potentialMatch = (index + array->zero) - array->size;
    if (potentialMatch >= array->zero) {
      return NULL; // Array doesn't have enough space
    } else {
      return &(array->array[potentialMatch]);
    }
  } else {
    return &(array->array[array->zero + index]);
  }
}



howderek_array_value_t howderek_array_get(struct howderek_array* array,
                                          size_t index) {
  howderek_array_value_t* match = howderek_array_at(array, index);
  if (match == NULL || index >= array->size) {
    return HOWDEREK_ARRAY_EMPTY_VALUE;
  } else {
    return *match;
  }
}

void __howderek_array_set(struct howderek_array* array,
                          howderek_array_value_t* position,
                          howderek_array_value_t value) {
  if (*position == HOWDEREK_ARRAY_EMPTY_VALUE
      && value != HOWDEREK_ARRAY_EMPTY_VALUE) {
    array->count++;
  } else if (value == HOWDEREK_ARRAY_EMPTY_VALUE) {
    array->count = (array->count) ? array->count - 1 : 0;
  }
  *position = value;
}



howderek_array_value_t* howderek_array_set(struct howderek_array* array,
                                           size_t index,
                                           howderek_array_value_t value) {
  if (array == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_array: howderek_array_set called on array=NULL");
    return NULL;
  }
  howderek_array_value_t* position;
  while ((position = howderek_array_at(array, index)) == NULL) {
    howderek_array_grow(array, (array->size + 1) * 2);
  }
  __howderek_array_set(array, position, value);
  if (index > array->end) {
    array->end = index;
  }
  return position;
}



void howderek_array_swap(struct howderek_array* array,
                         size_t left,
                         size_t right) {
  howderek_array_value_t leftValue = howderek_array_get(array, left);
  howderek_array_value_t rightValue = howderek_array_get(array, right);
  howderek_array_set(array, left, rightValue);
  howderek_array_set(array, right, leftValue);
}



void howderek_array_clear(struct howderek_array* array, int freeData) {
  if (array != NULL) {
    if (freeData) {
      howderek_array_value_t* iter;
      while (howderek_array_iterate(array, &iter)) {
        free(iter);
      }
    }
    free(array->array);
    array->count = 0;
    array->array = NULL;
    howderek_array_grow(array, 1);
  }
}



void howderek_array_destroy(struct howderek_array* array, int freeData) {
  howderek_array_value_t* iter;
  if (array != NULL) {
    if (freeData) {
      while (howderek_array_iterate(array, &iter)) {
        free(*iter);
      }
    }
    free(array->array);
    free(array);
  }
}


howderek_array_value_t howderek_array_remove(struct howderek_array* array, size_t index) {
  howderek_array_value_t* position = howderek_array_at(array, index);
  howderek_array_value_t removedValue = *position;
  __howderek_array_set(array, position, HOWDEREK_ARRAY_EMPTY_VALUE);
  if (index == array->end) {
    size_t pos;
    // We have to find the new last value
    for (pos = array->size - 1 ;pos != SIZE_MAX; pos--) {
      if (howderek_array_get(array, pos) != HOWDEREK_ARRAY_EMPTY_VALUE) {
        array->end = pos;
      }
    }
  }
  return removedValue;
}



howderek_array_value_t* howderek_array_push(struct howderek_array* array,
                                            howderek_array_value_t value) {
  return howderek_array_set(array, array->count, value);
}



howderek_array_value_t howderek_array_pop(struct howderek_array* array) {
  return howderek_array_remove(array, array->count - 1);
}



howderek_array_value_t* howderek_array_insert(struct howderek_array* array,
                                              size_t position,
                                              howderek_array_value_t value) {
  howderek_array_slide(array, position, array->end, position + 1);
  return howderek_array_set(array, position, value);
}



howderek_array_value_t howderek_array_dequeue(struct howderek_array* array) {
  howderek_array_value_t value;
  value = array->array[array->zero];
  array->array[array->zero] = HOWDEREK_ARRAY_EMPTY_VALUE;
  if (value != HOWDEREK_ARRAY_EMPTY_VALUE) {
    array->count--;
  }
  array->zero = (array->zero + 1) % array->size;
  return value;
}



howderek_array_value_t* howderek_array_slide(struct howderek_array* array,
                                             size_t fromBegin,
                                             size_t fromEnd,
                                             size_t to) {
  if (array == NULL) {
    return NULL;
  }
  size_t i;
  howderek_array_value_t* newPos;
  for (i = 0; i < (fromEnd - fromBegin); i++) {
    howderek_array_value_t value = howderek_array_get(array, fromBegin + i);
    howderek_array_set(array, fromBegin + i, HOWDEREK_ARRAY_EMPTY_VALUE);
    newPos = howderek_array_set(array, to + i, value);
  }
  return newPos;
}


int howderek_array_iterate(struct howderek_array* array,
                           howderek_array_value_t** iter) {
  if (array == NULL || iter == NULL || *iter == NULL) {
    if (iter == NULL || array == NULL) {
      return 0;
    }
    *iter = &(array->array[array->zero]);
    return 1;
  }
  // Set the iterator to the address of the position in the array
  // that is (currentPosition + 1) mod array.size
  *iter = &(array->array[(((*iter) - array->array + 1) % array->size)]);
  if (*iter == &(array->array[array->zero])) {
    return 0;
  } else {
    return 1;
  }
}


void howderek_array_for_each(struct howderek_array* array,
                            void(*func)(struct howderek_array* array,
                                        size_t index,
                                        howderek_array_value_t* value)) {
  howderek_array_value_t* iter = NULL;
  size_t i;
  for (i = 0; howderek_array_iterate(array, &iter); i++) {
    if (*iter != HOWDEREK_ARRAY_EMPTY_VALUE) {
      func(array, i, iter);
    }
  }
}
