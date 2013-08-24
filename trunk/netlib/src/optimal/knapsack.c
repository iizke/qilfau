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
 * Method: Dynamic Programming
 * Ref: http://en.wikipedia.org/wiki/Knapsack_problem
 */
long knapsack01_solve1 (KNAPSACK01_T *ks) {
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
          //(ks->items)[i].is_chosen = 1;
        } else
          (ks->items)[i].is_chosen = 0;
      }
    }
  }

  return m[ks->nitems][ks->capacity];
}

/**
 * Knapsack01 problem sovling
 * Method: Meet in the middle
 * Ref: http://en.wikipedia.org/wiki/Knapsack_problem
 */
long knapsack01_solve2 (KNAPSACK01_T *ks){
  long m = 0;
  int i, j, n, l, h, s1 = 0, s2 = 0;
  long *w1 = NULL, *w2 = NULL;
  double *v1 = NULL, *v2 = NULL;
  char *b1 = NULL, *b2 = NULL;
  check_null_pointer(ks);
  n = ks->nitems;
  if (n > 16) {
    iprint(LEVEL_INFO, "Should not apply this method for n > 16\n");
    //return -1;
  }
  l = n/2;
  h = n - l;
  w1 = gc_malloc((1<<l)* sizeof (long));
  v1 = gc_malloc((1<<l)* sizeof (double));
  b1 = gc_malloc(l* sizeof (char));
  w2 = gc_malloc((1<<h)* sizeof (long));
  v2 = gc_malloc((1<<h)* sizeof (double));
  b2 = gc_malloc(h* sizeof (char));
  // compute combination
  for (i = 0; i < (1<<l); i++) {
    for (j = 0; j<l; j++){
      b1[j] = (i & (1<<j)) > 0? 1: 0;
      w1[i] += b1[j] * (ks->items)[j].weight;
      v1[i] += b1[j] * (ks->items)[j].value;
    }
  }
  for (i = 0; i < (1<<h); i++) {
    for (j = 0; j<h; j++){
      b2[j] = (i & (1<<j)) > 0? 1: 0;
      w2[i] += b2[j] * (ks->items)[l+j].weight;
      v2[i] += b2[j] * (ks->items)[l+j].value;
    }
  }
  // search the maximum value
  for (i = 0; i < (1<<l); i++) {
    for (j = 0; j < (1<<h); j++) {
      if (w1[i] + w2[j] > ks->capacity) continue;
      if (v1[i] + v2[j] <= m) continue;
      m = v1[i] + v2[j];
      s1 = i;
      s2 = j;
    }
  }

  for (i = 0; i<l; i++)
    b1[i] = (s1 & (1<<i)) > 0? 1: 0;
  for (i = 0; i<h; i++)
    b2[i] = (s2 & (1<<i)) > 0? 1: 0;
  // set up the selected items
  for (i = 0; i < ks->nitems; i++) {
    if ((i < l && b1[i] == 1) || (i >= l && b2[i-l] == 1))
      (ks->items)[i].is_chosen = 1;
  }
  gc_free(w1);gc_free(v1);gc_free(w2);gc_free(v2);gc_free(b1);gc_free(b2);
  return m;
}

int knapsack01_clean (KNAPSACK01_T *ks) {
  gc_free(ks->items);
  gc_free(ks);
  return SUCCESS;
}

int knapsack01_test () {
  KNAPSACK01_T ks;
  int i;
  long result;
  knapsack01_setup(&ks, 95, 5);
  knapsack01_insert_item(&ks, 40, 40);
  knapsack01_insert_item(&ks, 20, 20);
  knapsack01_insert_item(&ks, 50, 50);
  knapsack01_insert_item(&ks, 30, 30);
  knapsack01_insert_item(&ks, 10, 10);
  for (i=0; i<ks.nitems; i++)
    printf("Item %d, weight %ld, value %f, status %d\n", i, (ks.items)[i].weight,(ks.items)[i].value, (ks.items)[i].is_chosen);

  result = knapsack01_solve1(&ks);
  printf("Max value %ld \n", result);
  result = knapsack01_solve2(&ks);
  printf("Max value %ld \n", result);

  for (i=0; i<ks.nitems; i++)
    printf("Item %d, weight %ld, value %f, status %d\n", i, (ks.items)[i].weight,(ks.items)[i].value, (ks.items)[i].is_chosen);

  return SUCCESS;
}
