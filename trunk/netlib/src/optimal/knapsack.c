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
  ks->inserting_point = 0;
  //if (ks->items) gc_free(ks->items);
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
  (ks->items)[id].is_chosen = 0;
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
  (ks->items)[ks->inserting_point].is_chosen = 0;
  ks->inserting_point++;
  return SUCCESS;
}

/**
 * Knapsack01 problem sovling
 * Ref: http://en.wikipedia.org/wiki/Knapsack_problem#0.2F1_Knapsack_Problem
 */
long knapsack01_solve (KNAPSACK01_T *ks) {
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

  for (i=0; i < ks->nitems; i++){
    for (j = 0; j <= ks->capacity; j++) {
      long wi = (ks->items)[i].weight;
      if (j < wi){
        m[i+1][j] = m[i][j];
        (ks->items)[i].is_chosen = 0;
      }
      else {
        m[i+1][j] = m[i][j];
        if (m[i+1][j] < m[i][j - wi] + (ks->items)[i].value) {
          m[i+1][j] = m[i][j - wi] + (ks->items)[i].value;
          (ks->items)[i].is_chosen = 1;
        } else
          (ks->items)[i].is_chosen = 0;
      }
    }
  }

  return m[ks->nitems][ks->capacity];
}

int knapsack01_test () {
  KNAPSACK01_T ks;
  int i;
  long result;
  knapsack01_setup(&ks, 100, 5);
  knapsack01_insert_item(&ks, 10, 10);
  knapsack01_insert_item(&ks, 20, 20);
  knapsack01_insert_item(&ks, 30, 30);
  knapsack01_insert_item(&ks, 40, 40);
  knapsack01_insert_item(&ks, 50, 50);

  result = knapsack01_solve(&ks);
  printf("Max value %d \n", result);
    for (i=0; i<ks.nitems; i++)
      printf("Item %d, weight %ld, value %ld, status %d\n", i, (ks.items)[i].weight,(ks.items)[i].value, (ks.items)[i].is_chosen);

  return SUCCESS;
}
