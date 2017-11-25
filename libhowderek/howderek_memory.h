/*! \file howderek_memory.h
 *  \brief Memory management
*/

#ifndef H_LIBHOWDEREK_MEMORY_HASHMAP_H
#define H_LIBHOWDEREK_MEMORY_HASHMAP_H

#ifndef HOWDEREK_HASHMAP_CONFIG
#define HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS 3
#define HOWDEREK_MEMORY_HASHMAP_DEFAULT_SIZE 5
#endif

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"

struct howderek_memory_hashmap_node {
  const char* type;
  size_t size;
  void* parent;
  void* ptr;
  struct howderek_memory_hashmap_node* prev;
  struct howderek_memory_hashmap_node* next;
  uint8_t allocated;
};

struct howderek_memory_hashmap {
  size_t count;
  size_t size;
  size_t allocations;
  size_t deallocations;
  size_t resizes;
  int    __primeCount;
  struct howderek_memory_hashmap_node* tables[HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS];
};

void howderek_initalize_memory_management();

void* howderek_allocate(const char* type, void* parent, size_t size);

void* howderek_allocate_and_zero(const char* type, void* parent, size_t size);

struct howderek_memory_hashmap_node* howderek_memory_hash(void* ptr);

size_t howderek_object_memory_usage(void* parent);

void howderek_free(void* ptr);

void howderek_destroy(void* parent);

void howderek_display_memory_usage();

void howderek_clean_up();

void howderek_reset_memory_management();

#endif
