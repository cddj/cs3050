#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "libhowderek/howderek.h"
#include "libhowderek/howderek_kv.h"


int main(int argc, char** argv) {
  howderek_set_log_level(HOWDEREK_LOG_DEBUG);
  char* alphabet = "abcdefghijklmnopqrstuvwxz";
  howderek_log(HOWDEREK_LOG_DEBUG, "creating key-value store....");
  struct howderek_kv *kv = howderek_kv_create(0, 0);
  howderek_log(HOWDEREK_LOG_DEBUG, "assigning letters....");
  size_t i;
  for (i = 0; i < 50; i += 11) {
    howderek_kv_set(kv, i, alphabet + (i % 26));
  }
  howderek_log(HOWDEREK_LOG_DEBUG, "getting letters....");
  for (i = 0; i < 50; i += 11) {
    char* letter;
    letter = howderek_kv_get(kv, i);
    if (letter != NULL) {
      howderek_log(HOWDEREK_LOG_DEBUG, "%c", *letter);
    } else {
      howderek_log(HOWDEREK_LOG_DEBUG, "NULL");
    }
  }
  struct howderek_kv_iter iter;
  howderek_kv_fill_iter(kv, &iter);

  for (i = 0; howderek_kv_iterate(kv, &iter); i++);
  howderek_log(HOWDEREK_LOG_DEBUG, "letter %lu: %c\n", i, ((char*)*(iter.array_iter))[0]);
}
