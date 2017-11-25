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


int main(int argc, char** argv) {
  uint32_t verticies, edges;
  uint64_t* alreadyThere;
  srand(time(NULL));
  printf("Number of verticies: ");
  scanf("%u", &verticies);
  alreadyThere = calloc(sizeof(uint64_t), verticies);
  printf("Number of edges: ");
  scanf("%u", &edges);

  FILE* out = fopen("testfile.txt", "wb");
  fprintf(out, "%u", verticies);

  uint32_t start, end;
  start = 1;
  while (edges --> 0) {
    end = (random() % verticies) + 1;
    if (start != end && (alreadyThere[start] == 0 || alreadyThere[start] % end != 0)) {
      if (alreadyThere[start] == 0) {
        alreadyThere[start] = end;
      } else {
        alreadyThere[start] *= end;
      }
      fprintf(out, "\n(%u,%u)", start, end);
    } else {
      edges++;
    }
    uint32_t roll = (random() % 20) + 1;
    if (roll == 1) {
      // Critical failure! break the chain...
      start = (random() % verticies) + 1;
    } else if (roll > 1 && roll < 20) {
      // Usually just continue the chain.
      start = end;
    } else {
      // Critical success! Do nothing, so we'll create a branch
    }
  }

  printf("saved testfile.txt\n");

}
