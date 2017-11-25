/*! \file howderek_hashmaplist.h */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_hashmap.h"

struct howderek_hashmap* howderek_hashmap_create(size_t sqrt_size) {
  if (sqrt_size < HOWDEREK_HASHMAP_MIN_SIZE) {
    sqrt_size = HOWDEREK_HASHMAP_MIN_SIZE;
  }
  struct howderek_hashmap *newHashmap;
  newHashmap = malloc(sizeof(struct howderek_hashmap));
  size_t i = 0;
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    newHashmap->tables[i] = NULL;
  }
  newHashmap->__primeCount = sqrt_size;
  newHashmap->size = howderek_artisan_primes[sqrt_size];
  howderek_hashmap_clear(&newHashmap);
  return newHashmap;
}



 void howderek_hashmap_clear(struct howderek_hashmap** hashmap_ptr) {
   struct howderek_hashmap* hashmap = *hashmap_ptr;
   hashmap->count = 0;
   int i, j;
   for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
     if (hashmap->tables[i] != NULL) {
       free(hashmap->tables[i]);
     }
     hashmap->tables[i] = malloc(sizeof(struct howderek_hashmap_node) * hashmap->size);
     if (hashmap->tables[i] == NULL) {
       howderek_log(HOWDEREK_LOG_FATAL, "error in %s (malloc failed)", __func__);
     }
     for (j = 0; j < hashmap->size; j++) {
         hashmap->tables[i][j].key = HOWDEREK_HASHMAP_NULL_ID;
         hashmap->tables[i][j].value = 0;
      }
   }
}



void __howderek_hashmap_resize(struct howderek_hashmap** hashmap_ptr) {
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  struct howderek_hashmap* newHashmap = howderek_hashmap_create(hashmap->__primeCount + 1);
  int i, j;
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    for (j = 0; j < hashmap->size; j++) {
      if (hashmap->tables[i][j].key != HOWDEREK_HASHMAP_NULL_ID) {
       howderek_hashmap_add_with_flags(&newHashmap, hashmap->tables[i][j].key, hashmap->tables[i][j].value, hashmap->tables[i][j].flags);
      }
    }
  }
  howderek_hashmap_destroy(hashmap_ptr, 0);
  *hashmap_ptr = newHashmap;
}



struct howderek_hashmap_node* __howderek_hashmap_hash(struct howderek_hashmap* hashmap, howderek_hashmap_key_t key) {
  size_t hash_array[HOWDEREK_HASHMAP_HASH_FUNCTIONS];
  unsigned int i;
  hash_array[0] = key % hashmap->size;
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS - 1; i++) {
  /*
   * The hash functions
   * ---------------------
   *   h1 = key mod size
   *
   *         |  key    |
   *   h2 =  |  ----   | mod size
   *         |_ prime _|
   *
   *         |      key       |
   *   h3 =  |  ------------  | mod size
   *         |_ bigger prime _|
   *
   *         |        key          |
   *   h4 =  |  -----------------  | mod size
   *         |_ EVEN BIGGER PRIME _|
   *
   *   and so on....
   *
   * The ugly 'L's are the floor function
   */
    hash_array[i + 1] = (key / howderek_artisan_primes[hashmap->__primeCount + i]) % hashmap->size;
  }
  // Try to find that key
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    if (hashmap->tables[i][hash_array[i]].key == key) {
      return hashmap->tables[i] + hash_array[i];
    }
  }
  // If that key doesn't exist, find the first empty node
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    if (hashmap->tables[i][hash_array[i]].key == HOWDEREK_HASHMAP_NULL_ID) {
      return hashmap->tables[i] + hash_array[i];
    }
  }
  // If there are as many collisions as hashes, return NULL (needs resize)
  return NULL;
}



/*
 * The gist of the implementation of all the functions that
 * require hashing is as follows:
 *
 *   1. Calculate the result of each hashing function
 *   2. Iterate through each hash table to see if it matches
 *     a. If so, and we don't want it to, evict it to the next table
 *     b. If so, and we do want it to, manipulate it
 *   3. If no matches occured and we needed them to, resize the table
 *   4. Exit
 */
struct howderek_hashmap_node* howderek_hashmap_add_with_flags(struct howderek_hashmap** hashmap_ptr,
                                                   howderek_hashmap_key_t key,
                                                   howderek_hashmap_value_t value,
                                                   uint8_t flags) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_hashmap: howderek_hashmap_add_with_flags called on NULL hashmap");
    return 0;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  struct howderek_hashmap_node* node = __howderek_hashmap_hash(hashmap, key);
  if (node != NULL) {
    if (node->key != key) {
      hashmap->count++;
    }
    node->key = key;
    node->value = value;
    node->flags = flags;
  } else if (node == NULL || hashmap->count > (hashmap->size * HOWDEREK_HASHMAP_THRESHOLD)) {
    // If we are out of tables or if load factor is approaching saturation, increase size
    __howderek_hashmap_resize(hashmap_ptr);
    howderek_hashmap_add_with_flags(hashmap_ptr, key, value, flags);
  }
  return node;
}

struct howderek_hashmap_node* howderek_hashmap_set_static(struct howderek_hashmap** hashmap_ptr,
                                                   howderek_hashmap_key_t key,
                                                   howderek_hashmap_value_t value) {
  return howderek_hashmap_add_with_flags(hashmap_ptr, key, value, 0);
}

struct howderek_hashmap_node* howderek_hashmap_set(struct howderek_hashmap** hashmap_ptr,
                                                   howderek_hashmap_key_t key,
                                                   howderek_hashmap_value_t value) {
  return howderek_hashmap_add_with_flags(hashmap_ptr, key, value, 1);
}


int howderek_hashmap_remove(struct howderek_hashmap** hashmap_ptr, howderek_hashmap_key_t key) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_hashmap: howderek_hashmap_remove called on NULL hashmap");
    return 0;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  struct howderek_hashmap_node* node = __howderek_hashmap_hash(hashmap, key);
  // Does there exist a value here?
  if (node != NULL && node->value != 0) {
    if (HOWDEREK_HASHMAP_NODE_ALLOCATED(node->flags)) {
      free(node->value);
    }
    node->key = HOWDEREK_HASHMAP_NULL_ID;
    node->value = 0;
    hashmap->count--;
    return 1;
  } else {
    return 0;
  }
}



howderek_hashmap_value_t howderek_hashmap_get(struct howderek_hashmap** hashmap_ptr,
                                              howderek_hashmap_key_t key) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_hashmap: howderek_hashmap_get called on NULL hashmap");
    return NULL;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  struct howderek_hashmap_node* node = __howderek_hashmap_hash(hashmap, key);
  if (node != NULL && node->key == key) {
    return node->value; // Found it!
  } else {
    return NULL; // Did not find it
  }
}

void howderek_hashmap_for_each(struct howderek_hashmap** hashmap_ptr,
                               void (*func)(struct howderek_hashmap_node* node)) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  int i, j;
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    for (j = 0; j < hashmap->size; j++) {
      if (hashmap->tables[i][j].key != HOWDEREK_HASHMAP_NULL_ID) {
        func(&hashmap->tables[i][j]);
      }
    }
  }
}



void howderek_hashmap_for_each_sequential(struct howderek_hashmap** hashmap_ptr,
                                          void (*func)(struct howderek_hashmap_node* node)) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  size_t i = 0;
  howderek_hashmap_key_t j = HOWDEREK_HASHMAP_NULL_ID + 1;
  while (i < hashmap->count) {
    struct howderek_hashmap_node* node = howderek_hashmap_get(hashmap_ptr, j);
    if (node != NULL) {
      func(node);
      i++;
    }
    j++;
  }
}


void howderek_hashmap_for_each_value(struct howderek_hashmap** hashmap_ptr,
                               void (*func)(howderek_hashmap_value_t)) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  int i, j;
  for (i = 0; i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    for (j = 0; j < hashmap->size; j++) {
      if (hashmap->tables[i][j].key != HOWDEREK_HASHMAP_NULL_ID) {
        func(hashmap->tables[i][j].value);
      }
    }
  }
}


void howderek_hashmap_for_each_value_sequential(struct howderek_hashmap** hashmap_ptr,
                                                void (*func)(howderek_hashmap_value_t)) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  size_t i = 0;
  howderek_hashmap_key_t j = 1;
  while (i < hashmap->count) {
    struct howderek_hashmap_node* node = howderek_hashmap_get(hashmap_ptr, j);
    if (node != NULL) {
      func(&(node->value));
      i++;
    }
    j++;
  }
}


/**
 * Create iterator list
 *
 * \param hashmap  hashmap to iterate on
 * \param iter     iterator to set
 * \return         howderek_hashmap_iter
 */
void howderek_hashmap_fill_iter(struct howderek_hashmap** hashmap_ptr,
                                struct howderek_hashmap_iter* hashmap_iter) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  struct howderek_hashmap_iter* iter;
  struct howderek_hashmap_iter* end = NULL;
  size_t i, j, k = 0;
  hashmap_iter->next = NULL;
  hashmap_iter->node = NULL;
  for (i = 0; hashmap != NULL && i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    // First, free all the values (if required)
    for (j = 0; j < hashmap->size; j++) {
      if (hashmap->tables[i][j].key) {
        if (end != NULL && hashmap->tables[i][j].key > end->node->key) {
          struct howderek_hashmap_iter* newNext = malloc(sizeof(struct howderek_hashmap_iter));
          newNext->next = NULL;
          newNext->node = &(hashmap->tables[i][j]);
          end->next = newNext;
          end = end->next;
        } else {
          iter = hashmap_iter;
          while (iter != NULL) {
            if (iter->next == NULL) {
              iter->next = malloc(sizeof(struct howderek_hashmap_iter));
              iter->next->node = &(hashmap->tables[i][j]);
              iter->next->next = NULL;
              end = iter->next;
              break;
            } else if (iter->next->node->key < hashmap->tables[i][j].key) {
              struct howderek_hashmap_iter* newNext = malloc(sizeof(struct howderek_hashmap_iter));
              newNext->next = iter->next;
              newNext->node = &(hashmap->tables[i][j]);
              iter->next = newNext;
              break;
            }
            iter = iter->next;
          }
        }
      }
    }
  }
}

int howderek_hashmap_iterate(struct howderek_hashmap_iter* hashmap_iter) {
  if (hashmap_iter != NULL && hashmap_iter->next != NULL) {
    hashmap_iter->node = hashmap_iter->next->node;
    struct howderek_hashmap_iter* toFree = hashmap_iter->next;
    hashmap_iter->next = hashmap_iter->next->next;
    free(toFree);
    return 1;
  } else {
    return 0;
  }
}

void howderek_hashmap_destroy(struct howderek_hashmap** hashmap_ptr, uint8_t freeData) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  int i, j;
  for (i = 0; hashmap != NULL && i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    // First, free all the values (if required)
    for (j = 0; j < hashmap->size; j++) {
      if (HOWDEREK_HASHMAP_NODE_ALLOCATED(hashmap->tables[i][j].flags) && freeData) {
        free(hashmap->tables[i][j].value);
        hashmap->tables[i][j].value = 0;
      }
    }
    // Then, free the tables
    free(hashmap->tables[i]);
  }
  // Finally, the hashmap itself...
  free(hashmap);
  // ...and make sure we aren't pointing into random memory
  *hashmap_ptr = NULL;
}

struct howderek_hashmap* howderek_hashmap_import(FILE* input);



int howderek_hashmap_export(struct howderek_hashmap** hashmap_ptr, FILE* output);



void howderek_hashmap_display(struct howderek_hashmap** hashmap_ptr, uint8_t showTables) {
  if (hashmap_ptr == NULL || *hashmap_ptr == NULL) {
    printf("hashmap is NULL\n");
    return;
  }
  struct howderek_hashmap* hashmap = *hashmap_ptr;
  size_t i, j;
  size_t size = sizeof(struct howderek_hashmap*) + (sizeof(struct howderek_hashmap_node) * hashmap->size * HOWDEREK_HASHMAP_HASH_FUNCTIONS);
  printf("Meta:\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n"
         "| Count                 | Prime                           | Tables |  Size (without linked data) |\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n"
         "| %-21lu | Prime %-2d: %-21lu | %-6d | %21lu bytes |\n"
         "+-----------------------+---------------------------------+--------+-----------------------------+\n\n",
         hashmap->count, hashmap->__primeCount, hashmap->size, HOWDEREK_HASHMAP_HASH_FUNCTIONS, size);
  for (i = 0; showTables && i < HOWDEREK_HASHMAP_HASH_FUNCTIONS; i++) {
    printf("Hash Table %lu:\n"
           "+-----------------------+-----------------------+--------------------+\n"
           "| Index/Hash            | Key                   | Value              |\n"
           "+-----------------------+-----------------------+--------------------+\n", i);
    for (j = 0; j < hashmap->size; j++) {
      if (hashmap->tables[i][j].key != HOWDEREK_HASHMAP_NULL_ID) {
       printf("| %-21lu | %-21" HOWDEREK_HASHMAP_KEY_PRINTF_STR " | %-18" HOWDEREK_HASHMAP_VALUE_PRINTF_STR " |\n",
              j, hashmap->tables[i][j].key, hashmap->tables[i][j].value);
      }
    }
    printf("+-----------------------+-----------------------+--------------------+\n\n");
  }
}
