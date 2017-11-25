/*! \file howderek_array.c
    \brief Key-value store. Uses either a dynamic array or hashmap depending on how sparse the data is.
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "howderek.h"
#include "howderek_memory.h"
#include "howderek_array.h"
#include "howderek_hashmap.h"
#include "howderek_kv.h"


void __howderek_kv_to_hashmap(struct howderek_kv* kv, size_t size) {
  if (kv->type != HOWDEREK_KV_HASHMAP) {
    kv->hashmap = howderek_hashmap_create(howderek_optimal_prime(size));
    howderek_array_value_t* iter = NULL;
    size_t i;
    if (kv->array != NULL) {
      for (i = 0; howderek_array_iterate(kv->array, &iter); i++) {
        howderek_hashmap_set(&(kv->hashmap), (i + 1), iter[0]);
      }
      howderek_array_destroy(kv->array, 0);
      kv->array = NULL;
    }
  }
  kv->type = HOWDEREK_KV_HASHMAP;
}



void __howderek_kv_to_array(struct howderek_kv* kv, size_t size) {
  if (kv->type != HOWDEREK_KV_ARRAY) {
    kv->array = howderek_array_create(size);
    struct howderek_hashmap_iter iter;
    iter.next = NULL;
    iter.node = NULL;
    if (kv->hashmap != NULL) {
      howderek_hashmap_fill_iter(&(kv->hashmap), &iter);
      while (howderek_hashmap_iterate(&iter)) {
        howderek_array_set(kv->array, iter.node->key - 1, iter.node->value);
      }
      howderek_hashmap_destroy(&(kv->hashmap), 0);
      kv->hashmap = NULL;
    }
  }
  kv->type = HOWDEREK_KV_ARRAY;
}



struct howderek_kv* howderek_kv_create(size_t size, enum howderek_kv_containers type) {
  struct howderek_kv* newKV = malloc(sizeof(struct howderek_kv));
  newKV->count        = 0;
  newKV->size         = 0;
  newKV->threshold    = 0.80;
  newKV->expectedSize = size;
  newKV->array        = NULL;
  newKV->hashmap      = NULL;
  newKV->type         = 0;
  switch (type) {
    case HOWDEREK_KV_ARRAY:
      __howderek_kv_to_array(newKV, newKV->expectedSize);
    case HOWDEREK_KV_HASHMAP:
    default:
      __howderek_kv_to_hashmap(newKV, newKV->expectedSize);
  }
  return newKV;
}



howderek_array_value_t howderek_kv_get(struct howderek_kv* kv, size_t key) {
  if (kv == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_kv: howderek_kv_get called on kv=NULL");
    return HOWDEREK_ARRAY_EMPTY_VALUE;
  }
  switch (kv->type) {
    case HOWDEREK_KV_ARRAY:
      return howderek_array_get(kv->array, key);
    case HOWDEREK_KV_HASHMAP:
    default:
      return howderek_hashmap_get(&(kv->hashmap), (key + 1));
  }
}



void howderek_kv_set(struct howderek_kv* kv, size_t key, howderek_array_value_t value) {
  if (kv == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_kv: howderek_kv_set called on kv=NULL");
    return;
  }
  if (value == HOWDEREK_ARRAY_EMPTY_VALUE) {
    howderek_kv_delete(kv, key);
  }
  howderek_array_value_t current = howderek_kv_get(kv, key);
  if (current == NULL) {
    kv->count++;
    if (key > kv->size) {
      kv->size = key;
    }
    kv->zeroRatio = (float) kv->count / (float) kv->size;
    if (kv->zeroRatio > kv->threshold && kv->type == HOWDEREK_KV_HASHMAP) {
      howderek_log(HOWDEREK_LOG_DEBUG, "key-value store at %p now using array", kv);
      __howderek_kv_to_array(kv, kv->size);
    } else if (kv->zeroRatio <= kv->threshold && kv->type == HOWDEREK_KV_ARRAY) {
      howderek_log(HOWDEREK_LOG_DEBUG, "key-value store at %p is now using hashmap", kv);
      __howderek_kv_to_hashmap(kv, kv->size);
    }
  }
  switch (kv->type) {
    case HOWDEREK_KV_ARRAY:
      howderek_array_set(kv->array, key, value);
      return;
    case HOWDEREK_KV_HASHMAP:
      howderek_hashmap_set(&(kv->hashmap), key, value);
      return;
  }
}



howderek_array_value_t howderek_kv_delete(struct howderek_kv* kv, size_t key) {
  if (kv == NULL) {
    howderek_log(HOWDEREK_LOG_ERROR, "libhowderek_kv: howderek_kv_delete called on kv=NULL");
    return NULL;
  }
  howderek_array_value_t current = howderek_kv_get(kv, key);
  if (current == NULL) {
    kv->count--;
    if (key == kv->size) {
      long long int i;
      for (i = kv->size; i >= 0; i--) {
        if (howderek_kv_get(kv, i) != HOWDEREK_ARRAY_EMPTY_VALUE) {
          kv->size = i;
          break;
        }
      }
    }
    kv->zeroRatio = (float) kv->count / (float) kv->size;
    if (kv->zeroRatio > kv->threshold && kv->type == HOWDEREK_KV_HASHMAP) {
      __howderek_kv_to_array(kv, kv->size);
    } else if (kv->zeroRatio <= kv->threshold && kv->type == HOWDEREK_KV_ARRAY) {
      __howderek_kv_to_hashmap(kv, kv->size);
    }
  }
  return current;
}



void howderek_kv_fill_iter(struct howderek_kv* kv, struct howderek_kv_iter* iter) {
  iter->key = 0;
  iter->value = HOWDEREK_ARRAY_EMPTY_VALUE;
  switch (kv->type) {
    case HOWDEREK_KV_ARRAY:
      iter->array_iter = NULL;
      iter->hashmap_iter.next = NULL;
      iter->hashmap_iter.node = NULL;
      return;
    case HOWDEREK_KV_HASHMAP:
    default:
      iter->array_iter = NULL;
      howderek_hashmap_fill_iter(&(kv->hashmap), &(iter->hashmap_iter));
      return;
  }
}



int howderek_kv_iterate(struct howderek_kv* kv, struct howderek_kv_iter* iter) {
  int success;
  switch (kv->type) {
    case HOWDEREK_KV_ARRAY:
      success = howderek_array_iterate(kv->array, &(iter->array_iter));
      if (iter->array_iter[0] == HOWDEREK_ARRAY_EMPTY_VALUE) {
        while (iter->array_iter[0] == HOWDEREK_ARRAY_EMPTY_VALUE && success) {
          iter->key++;
          success = howderek_array_iterate(kv->array, &(iter->array_iter));
        }
      }
      if (success) {
        iter->key++;
        iter->value = iter->array_iter[0];
      }
      return success;
    case HOWDEREK_KV_HASHMAP:
    default:
      success = howderek_hashmap_iterate(&(iter->hashmap_iter));
      if (success) {
        iter->key = iter->hashmap_iter.node->key;
        iter->value = iter->hashmap_iter.node->value;
      }
      return success;
  }
}


void howderek_kv_destroy(struct howderek_kv* kv, int freeData) {
  if (freeData) {
    struct howderek_kv_iter iter;
    howderek_kv_fill_iter(kv, &iter);
    while(howderek_kv_iterate(kv, &iter)) {
      if (iter.value) {
        free(iter.value);
      }
    }
  }
  free(kv->array);
  free(kv);
}
