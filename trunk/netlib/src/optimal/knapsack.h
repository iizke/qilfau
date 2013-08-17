/*
 * knapsack.h
 *
 *  Created on: Aug 17, 2013
 *      Author: iizke
 */

#ifndef KNAPSACK_H_
#define KNAPSACK_H_

typedef struct knapsack_item {
  long weight;
  double value;
  int is_chosen;
} KNAPSACK_ITEM;

typedef struct knapsack01_t {
  long capacity;
  int nitems;
  int inserting_point;
  KNAPSACK_ITEM *items;
} KNAPSACK01_T;

int knapsack01_init (KNAPSACK01_T **, long capacity, int nitems);
int knapsack01_setup (KNAPSACK01_T *, long c, int n);

int knapsack01_set_item (KNAPSACK01_T*, int id, long weight, double value);
int knapsack01_insert_item (KNAPSACK01_T*, long weight, double value);

#endif /* KNAPSACK_H_ */
