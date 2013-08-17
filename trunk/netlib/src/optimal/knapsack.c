/*
 * knapsack.c
 *
 *  Created on: Aug 17, 2013
 *      Author: iizke
 */

#include "error.h"
#include "knapsack.h"

int knapsack01_init (KNAPSACK01_T **ks, long capacity, int nitems) {
  check_null_pointer(ks);
  if (!(*ks))
    *ks = gc_malloc(sizeof(KNAPSACK01_T));
  if (!(*ks))
    return ERR_MALLOC_FAIL;
  knapsack01_setup(*ks, capacity, nitems);
  return SUCCESS;
}

int knapsack01_setup (KNAPSACK01_T *ks, long capacity, int nitems) {
  check_null_pointer(ks);
  ks->capacity = capacity;
  ks->nitems = nitems;
  if (ks->items) gc_free(ks->items);
  ks->items = gc_malloc(sizeof(KNAPSACK_ITEM)*nitems);
  if (!ks->items) return ERR_MALLOC_FAIL;
  return SUCCESS;
}

int knapsack01_set_item (KNAPSACK01_T* ks, int id, long weight, double value) {
  check_null_pointer(ks);
  check_null_pointer(ks->items);
  if (id >= ks->nitems) return ERR_INVALID_VAL;
  (ks->items)[id].weight = weight;
  (ks->items)[id].value = value;
  return SUCCESS;
}

int knapsack01_insert_item (KNAPSACK01_T* ks, long weight, double value) {
  check_null_pointer(ks);
  if (ks->inserting_point >= ks->nitems) {
    // reach maximum number of objects
    return ERR_KNAPSACK_INSERT_FAIL;
  }
  (ks->items)[ks->inserting_point].value = value;
  (ks->items)[ks->inserting_point].weight = weight;
  ks->inserting_point++;
  return SUCCESS;
}

/**
 * Knapsack01 problem sovling
 * Ref: http://en.wikipedia.org/wiki/Knapsack_problem#0.2F1_Knapsack_Problem
 */
int knapsack01_solve (KNAPSACK01_T *ks) {
  long** m = NULL;
  long w = 0;
  long i = 0, j=0;
  check_null_pointer(ks);

  m = gc_malloc((ks->nitems+1) * sizeof(long*));
  for (i = 0; i <= ks->nitems; i++) {
    m[i] = gc_malloc((ks->capacity+1) * sizeof(long));
  }

  for (w = 0; w <= ks->capacity; w++) {
    m[0][w] = 0;
  }

  for (i=1; i <= ks->nitems; i++){
    for (j = 0; j <= ks->capacity; j++) {
      long wi = (ks->items)[i-1].weight;
      if (j < wi){
        m[i][j] = m[i-1][j];
        (ks->items)[i-1].is_chosen = 0;
      }
      else {
        m[i][j] = m[i-1][j];
        if (m[i][j] < m[i-1][j - wi] + (ks->items)[i-1].value) {
          m[i][j] = m[i-1][j - wi] + (ks->items)[i-1].value;
          (ks->items)[i-1].is_chosen = 1;
        }
        else {
          (ks->items)[i-1].is_chosen = 0;
        }
      }
    }
  }

  return SUCCESS;
}
