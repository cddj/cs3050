#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#include "howderek.h"
#include "howderek_memory.h"

static struct howderek_memory_hashmap* __howderek_global_memory = NULL;

void __dump_on_signal(int signo) {
  howderek_log(HOWDEREK_LOG_FATAL, "Recieved SIGQUIT, displaying memory...");
  if (signo == 3) {
    howderek_display_memory_usage();
    exit(1);
  }
}

void __howderek_memory_push(struct howderek_memory_hashmap_node* node);

void __howderek_memory_hashmap_resize(size_t size) {
  struct howderek_memory_hashmap* oldMemories = __howderek_global_memory;
  __howderek_global_memory = malloc(sizeof(struct howderek_memory_hashmap));
  size_t i = 0;
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    __howderek_global_memory->tables[i] = NULL;
  }
  __howderek_global_memory->__primeCount = size;
  __howderek_global_memory->size         = howderek_artisan_primes[size];
  howderek_reset_memory_management();
  if (oldMemories != NULL) {
    int i, j;
    for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
      for (j = 0; j < oldMemories->size; j++) {
        if (oldMemories->tables[i][j].parent != NULL) {
         __howderek_memory_push(&oldMemories->tables[i][j]);
        }
      }
    }
    for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
      free(oldMemories->tables[i]);
    }
  }
}



void __howderek_memory_push(struct howderek_memory_hashmap_node* node) {
  struct howderek_memory_hashmap_node* position;
  while ((position = howderek_memory_hash(node->ptr)) == NULL) {
    __howderek_memory_hashmap_resize(__howderek_global_memory->__primeCount + 1);
  }
  struct howderek_memory_hashmap_node* parent;
  while ((parent = howderek_memory_hash(node->parent)) == NULL) {
    __howderek_memory_hashmap_resize(__howderek_global_memory->__primeCount + 1);
  }

  // Pointers!
  if (parent->ptr == NULL) {
    parent->type = node->type;
    parent->ptr = node->parent;
    parent->parent =node->parent;
    __howderek_global_memory->count++;
  }
  if (parent->prev) {
    position->prev = parent->prev;
    position->prev->next = position;
  }
  parent->prev = position;
  position->next = parent;
  // Attributes
  position->size = node->size;
  position->parent = node->parent;
  position->type = node->type;
  position->ptr = node->ptr;
  position->allocated = node->allocated;
  __howderek_global_memory->count++;
}



void howderek_initalize_memory_management() {
  if (signal(3, __dump_on_signal) == SIG_ERR) {
    howderek_log(HOWDEREK_LOG_DEBUG, "failed to setup default SIGQUIT signal handler");
  }
  __howderek_memory_hashmap_resize(HOWDEREK_MEMORY_HASHMAP_DEFAULT_SIZE);
  __howderek_global_memory->resizes = 0;
}



void* howderek_allocate(const char* type, void* parent, size_t size) {
  struct howderek_memory_hashmap_node newNode;
  newNode.type = type;
  newNode.size = size;
  newNode.ptr = malloc(size);
  newNode.allocated = 1;
  if (parent == NULL) {
    newNode.parent = newNode.ptr;
  }
  __howderek_global_memory->allocations++;
  __howderek_memory_push(&newNode);
  return newNode.ptr;
}



void* howderek_allocate_and_zero(const char* type, void* parent, size_t size) {
  void* ptr = howderek_allocate(type, parent, size);
  memset(ptr, 0, size);
  return ptr;
}



struct howderek_memory_hashmap_node* howderek_memory_hash(void* ptr) {
  size_t hash_array[HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS];
  size_t i;
  hash_array[0] = (size_t) ptr % __howderek_global_memory->size;
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS - 1; i++) {
    hash_array[i + 1] = ((size_t) ptr / howderek_artisan_primes[__howderek_global_memory->__primeCount + i]) % __howderek_global_memory->size;
  }
  // Try to find that key
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    if (__howderek_global_memory->tables[i][hash_array[i]].ptr == ptr) {
      return &(__howderek_global_memory->tables[i][hash_array[i]]);
    }
  }
  // If that key doesn't exist, find the first empty node
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    if (__howderek_global_memory->tables[i][hash_array[i]].ptr == NULL) {
      return &(__howderek_global_memory->tables[i][hash_array[i]]);
    }
  }
  // If there are as many collisions as hashes, return NULL (needs resize)
  return NULL;
}



size_t howderek_object_memory_usage(void* parent) {
  struct howderek_memory_hashmap_node* iter = howderek_memory_hash(parent);
  size_t result = 0;
  while (iter != NULL) {
    iter = iter->prev;
    result += iter->size;
  }
  return result;
}



void howderek_free(void* ptr) {
  struct howderek_memory_hashmap_node* pos = howderek_memory_hash(ptr);
  struct howderek_memory_hashmap_node* head = howderek_memory_hash(pos->parent);
  if (pos == NULL || pos->ptr == NULL) {
    // This was probably not allocated by howderek_allocate
    return;
  }
  if (head->prev == pos) {
    head->prev = pos->prev;
  }
  if (pos->prev != NULL) {
    pos->prev->next = pos->next;
  }
  if (pos->next != NULL) {
    pos->next->prev = pos->prev;
  }
  // Make sure we have something to free (not parents or NULL)
  if (pos->allocated && pos->ptr != NULL) {
    free(pos->ptr);
    pos->ptr    = NULL;
    pos->allocated = 0;
  }
  pos->parent = NULL;
  pos->next   = NULL;
  pos->prev   = NULL;
  pos->type   = NULL;
  __howderek_global_memory->count--;
  __howderek_global_memory->deallocations++;
}



void howderek_destroy(void* parent) {
  struct howderek_memory_hashmap_node* head = howderek_memory_hash(parent);
  struct howderek_memory_hashmap_node* iter = head->prev;
  struct howderek_memory_hashmap_node* oldIter;
  while (iter != NULL) {
    oldIter = iter;
    iter = iter->prev;
    if (oldIter->parent == parent) {
      howderek_free(oldIter->ptr);
    }
  }
  howderek_free(parent);
}



void howderek_display_memory_usage() {
  size_t i, j, k;
  size_t objects = HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS;
  size_t size = sizeof(struct howderek_memory_hashmap*) + (sizeof(struct howderek_memory_hashmap_node) * __howderek_global_memory->size * HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS);
  printf("Memory Hashmap Metadata:\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n"
         "| Count                 | Prime                           | Tables |  Size                       |\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n"
         "| %-21lu | Prime %-2d: %-21lu | %-6d | %21lu bytes |\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n\n",
         __howderek_global_memory->count, __howderek_global_memory->__primeCount, __howderek_global_memory->size, HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS, size);
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    for (j = 0; j < __howderek_global_memory->size; j++) {
      struct howderek_memory_hashmap_node* current = &__howderek_global_memory->tables[i][j];
      if (current->parent == current->ptr && current->parent != NULL) {
       printf("\nparent %s* at %p\n---------------------------------------------------------------------\n", current->type, current->ptr);
       struct howderek_memory_hashmap_node* iter = current->prev;
       size_t totalSize = 0;
       for (k = 0; iter != NULL; k++) {
         printf("child %lu: %lu byte %s at %p\n", k, iter->size, iter->type, iter->ptr);
         totalSize += iter->size;
         iter = iter->prev;
       }
       printf("total size: %lu bytes\n", totalSize);
       size += totalSize;
       objects++;
      }
    }
  }
  printf(HOWDEREK_BOLD "\nallocated memory: %lu bytes in %lu objects (%lu unique pointers)\n\n" HOWDEREK_RESET,
         size, objects, HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS + __howderek_global_memory->count);
}

void howderek_reset_memory_management() {
  __howderek_global_memory->count = 0;
  __howderek_global_memory->allocations = 0;
  __howderek_global_memory->deallocations = 0;
  __howderek_global_memory->resizes++;
  size_t i, j;
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    if (__howderek_global_memory->tables[i] != NULL) {
      free(__howderek_global_memory->tables[i]);
    }
    __howderek_global_memory->tables[i] = malloc(sizeof(struct howderek_memory_hashmap_node) * __howderek_global_memory->size);
    if (__howderek_global_memory->tables[i] == NULL) {
      howderek_log(HOWDEREK_LOG_FATAL, "error in %s (malloc failed)", __func__);
    }
    for (j = 0; j < __howderek_global_memory->size; j++) {
      __howderek_global_memory->tables[i][j].type = NULL;
      __howderek_global_memory->tables[i][j].parent = NULL;
      __howderek_global_memory->tables[i][j].prev = NULL;
      __howderek_global_memory->tables[i][j].ptr = NULL;
      __howderek_global_memory->tables[i][j].size = 0;
      __howderek_global_memory->tables[i][j].allocated = 0;
     }
  }
}

void howderek_clean_up() {
  size_t i, j, k;
  size_t objects = HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS;
  size_t size = sizeof(struct howderek_memory_hashmap*) + (sizeof(struct howderek_memory_hashmap_node) * __howderek_global_memory->size * HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS);
  for (i = 0; i < HOWDEREK_MEMORY_HASHMAP_HASH_FUNCTIONS; i++) {
    for (j = 0; j < __howderek_global_memory->size; j++) {
      struct howderek_memory_hashmap_node* current = &__howderek_global_memory->tables[i][j];
      if (current->allocated) {
        free(current->ptr);
        size += current->size;
        objects += 1;
      }
    }
    free(__howderek_global_memory->tables[i]);
  }
  howderek_log(HOWDEREK_LOG_DEBUG, "%lu objects (%lu bytes) freed by howderek_clean_up()", objects, size);
}
