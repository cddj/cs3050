#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "howderek_memory.h"
#include "howderek_skiplist.h"


howderek_skip_value_t __howderek_skip_default_compare(struct howderek_skip_node* left, struct howderek_skip_node* right) {
  if (left != NULL && right != NULL) {
    return left->id - right->id;
  } else {
    return 1;
  }
}



void __howderek_skip_default_mutation(struct howderek_skip_node* left, struct howderek_skip_node* right) {
  if (right->data != NULL) {
    free(left->data);
    left->data = right->data;
  }
}



struct howderek_skip* howderek_skip_create_custom(howderek_skip_value_t (*compareFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right), void (*mutationFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right)) {
  struct howderek_skip* list;
  list = howderek_allocate_and_zero("skiplist", &list, sizeof(struct howderek_skip));
  if (list == 0) {
    fprintf(stderr, "fatal error in %s:%d - out of memory\n", __FILE__, __LINE__);
    exit(ENOMEM);
  }

  // Set compare function
  if (compareFunction != NULL) {
    list->compareFunction = compareFunction;
  } else {
    list->compareFunction = __howderek_skip_default_compare;
  }

  // Set mutation function
  if (mutationFunction != NULL) {
    list->mutationFunction = mutationFunction;
  } else {
    list->mutationFunction = __howderek_skip_default_mutation;
  }

  size_t i;
  list->head = howderek_allocate_and_zero("skiplist->head", &list, sizeof(struct howderek_skip_node));

  list->head->next = howderek_allocate_and_zero("skiplist_node", list->head, sizeof(struct howderek_skip_node*) * HOWDEREK_SKIP_MAX_LEVEL + 1);
  for (i = 0; i < HOWDEREK_SKIP_MAX_LEVEL; i++) {
    list->head->next[i] = list->head;
  }

  list->head->id = 0;
  list->head->data = NULL;

  list->ops = 0;
  list->count = 0;
  list->level = 0;

  return list;
}



struct howderek_skip* howderek_skip_create() {
  return howderek_skip_create_custom(NULL, NULL);
}



void howderek_skip_destroy(struct howderek_skip* list, int freeData) {
  struct howderek_skip_node *iter = list->head->next[0];
  struct howderek_skip_node *old = list->head;

  while (iter->next[0] != list->head) {
    old = iter;
    iter = iter->next[0];
    howderek_free(old->next);
    if (old->data != NULL && freeData == 1) {
      free(old->data);
    }
    howderek_free(old);
  }
  howderek_free(iter->next);
  howderek_free(iter);

  howderek_free(list->head->next);
  howderek_free(list->head);
  howderek_free(list);
}



void howderek_skip_set_compare_function(struct howderek_skip** list, howderek_skip_value_t (*compareFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right)) {
  struct howderek_skip* newList = howderek_skip_create_custom(compareFunction, NULL);

  if (list == NULL || *list == NULL) {
    return;
  }

  struct howderek_skip_node *iter = (*list)->head;
  while (iter->next[0] != (*list)->head) {
    iter = iter->next[0];
    howderek_skip_push(newList, iter->id, iter->data);
  }

  howderek_skip_destroy(*list, 0);
  *list = newList;
}



void howderek_skip_set_mutation_function(struct howderek_skip** list, void (*mutationFunction)(struct howderek_skip_node* left, struct howderek_skip_node* right)) {
  (*list)->mutationFunction = mutationFunction;
}



void howderek_skip_push(struct howderek_skip* list, howderek_skip_value_t id, void* data) {
  int newLevel = 0;
  int i = 0;
  int currentLevel;
  struct howderek_skip_node *update[HOWDEREK_SKIP_MAX_LEVEL + 2];
  struct howderek_skip_node *iter = list->head;
  struct howderek_skip_node newNode;

  newNode.id = id;
  newNode.data = data;

  list->ops++;
  
  /*
  * Traverse the skip list
  *
  * S---->X             O           Starts at the top, tries to stay
  * O   O X-X-X-->X     O           as high as possible while moving
  * O O O O O O   X     O O         to the largest value still greater
  * O O O O O O O X-F O O O         than (value)
  *
  * During the traversal, the furthest desination of each link is
  * recorded into update. That array is used to update the links
  * after the correct position is found.
  */
  for (currentLevel = list->level; currentLevel >= 0; currentLevel--) {
    while (iter->next[currentLevel] != list->head) {
      if (list->compareFunction(iter->next[currentLevel], &newNode) >= 0) {
        break;
      } else {
        // Follow link
        iter = iter->next[currentLevel];
      }
    }
    // before dropping levels, remember the furthest link on this level
    update[currentLevel] = iter;
  }
  iter = iter->next[0];

  // Determine if this node exists already, and mutate if so
  if (iter != list->head) {
    if (list->compareFunction(iter, &newNode) == 0) {
      list->mutationFunction(iter, (void*) &newNode);
      return;
    }
  }

  // Coin flips for the height of this node
  while (rand() % 4 == 0 && newLevel < HOWDEREK_SKIP_MAX_LEVEL) {
    newLevel++;
  }

  // Get the next level, credits to http://ticki.github.io/blog/skip-lists-done-right/
  // for the O(1) random suggestion
  //newLevel = __builtin_ffs(rand() & (HOWDEREK_SKIP_MAX_LEVEL - 1));

  // If height changed, initalize new level
  if (newLevel > list->level) {
    for (i = list->level + 1; i <= newLevel; i++) {
      update[i] = list->head;
    }
    list->level = newLevel;
  }

  // Set forward links on the new node
  iter = howderek_allocate_and_zero("skiplist_node", list->head, sizeof(struct howderek_skip_node));
  iter->next = howderek_allocate_and_zero("skiplist_node", list->head, sizeof(struct howderek_skip_node*) * newLevel + 1);
  iter->id = id;
  iter->data = data;
  iter->level = newLevel;
  for (i = 0; i <= newLevel; i++) {
    iter->next[i] = update[i]->next[i];
    update[i]->next[i] = iter;
  }

  list->count++;

}



int howderek_skip_delete(struct howderek_skip* list, howderek_skip_value_t id, void* data) {
  int i = 0;
  int currentLevel;
  struct howderek_skip_node *update[HOWDEREK_SKIP_MAX_LEVEL + 2];
  struct howderek_skip_node *iter = list->head;
  struct howderek_skip_node newNode;

  if (iter == NULL) {
    return 1;
  }

  newNode.id = id;
  newNode.data = data;

  list->ops++;
  
  // Traverse the skip list
  for (currentLevel = list->level; currentLevel >= 0; currentLevel--) {
    while (iter != NULL && iter->next[currentLevel] != list->head) {
      if (list->compareFunction(iter->next[currentLevel], &newNode) >= 0) {
        break;
      } else {
        // Follow link
        iter = iter->next[currentLevel];
      }
      update[currentLevel] = iter;
    }
    // before dropping levels, remember the furthest link on this level
    update[currentLevel] = iter;
  }
  iter = iter->next[0];

  if (iter == list->head || list->compareFunction(iter, &newNode) != 0) {
      return 0;
  }

  // Detach the node from the list by updating the links that attach to it
  for (i = 0; i <= list->level && update[i]->next[i] == iter; i++) {
    update[i]->next[i] = iter->next[i];
  }

  if (data != NULL) {
    free(iter->data);
  }
  howderek_free(iter);
  list->count--;

  // Update list level in case that node was the tallest
  while (list->level > 0 && list->head->next[list->level] != list->head) {
    list->level--;
  }
  return 1;
}

void howderek_skip_update(struct howderek_skip* list, howderek_skip_value_t id, void* oldData, void* newData) {
  howderek_skip_delete(list, id, oldData);
  howderek_skip_push(list, id, newData);
}

struct howderek_skip_node* howderek_skip_search(struct howderek_skip* list, howderek_skip_value_t id, void* data) {
  struct howderek_skip_node *iter = list->head;
  long int currentLevel;
  struct howderek_skip_node newNode;

  newNode.id = id;
  newNode.data = data;

  // Traverse the skip list
  for (currentLevel = list->level; currentLevel >= 0; currentLevel--) {
    while (iter != list->head) {
      if (list->compareFunction(iter, &newNode) >= 0) {
        break;
      }
      iter = iter->next[currentLevel];
    }
  }
  iter = iter->next[0];

  if (iter != list->head) {
    if (list->compareFunction(iter, &newNode) == 0) {
      return iter;
    }
  }
  return NULL;
}

struct howderek_skip_node* howderek_skip_index(struct howderek_skip* list, size_t i) {
  struct howderek_skip_node* iter = list->head;
  for (;i > 0 && iter != NULL; i--) {
    iter = iter->next[0];
  }
  if (i == 0) {
    return iter->next[0];
  } else {
    return NULL;
  }
}

int howderek_skip_delete_index(struct howderek_skip* list, size_t i, int freeData) {
  struct howderek_skip_node* toDelete = howderek_skip_index(list, i);
  if (toDelete == NULL) {
    return 0;
  } else {
    return howderek_skip_delete(list, toDelete->id, (freeData) ? toDelete->data : NULL);
  }
}


void howderek_skip_display(struct howderek_skip* list) {
  //Error checks
  if (list == NULL) {
    printf("NULL (list not created)\n");
    return;
  }
  if (list->head->next[0] == list->head) {
    printf("Empty list\n");
    return;
  }

  struct howderek_skip_node *iter = list->head;
  printf("Operations: " HOWDEREK_SKIP_VALUE_PRINTF_STR "; Count: " HOWDEREK_SKIP_VALUE_PRINTF_STR " mutations)\n\nList:\n", list->ops, list->count);
  while (iter->next[0] != list->head) {
    iter = iter->next[0];
    printf("(" HOWDEREK_SKIP_VALUE_PRINTF_STR ", %d)\n", iter->id, iter->level);
  }
  printf("\n");
}



struct howderek_skip* howderek_skip_import(FILE* input) {
  struct howderek_skip* list = howderek_skip_create();
  while (input != NULL) {
    howderek_skip_value_t value;
    int numberParsed = fscanf(input, HOWDEREK_SKIP_VALUE_PRINTF_STR, &value);
    // fscanf should return 1 if successful because it should read 1 value
    if (numberParsed != 1) {
      if (fgetc(input) != EOF) {
        exit(6);
      }
      break;
    }
    howderek_skip_push(list, value, NULL);
  }
  return list;
}



int howderek_skip_export(struct howderek_skip* list, FILE* input) {
  struct howderek_skip_node *iter = list->head;
  while (iter->next[0] != list->head) {
    iter = iter->next[0];
    fprintf(input, HOWDEREK_SKIP_VALUE_PRINTF_STR "\n", iter->id);
  }
  return 0;
}
