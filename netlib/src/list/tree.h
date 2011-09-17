/**
 * @file tree.h
 * Tree data structures
 * @date Created on: Aug 29, 2011
 * @author iizke
 */

#ifndef TREE_H_
#define TREE_H_

#include "def.h"

typedef struct tree_node {
  struct tree_node *left;
  struct tree_node *right;
  struct tree_node *mom;
} TREE_NODE;

int tree_insert_mom (TREE_NODE* mom, TREE_NODE* left, TREE_NODE* right);
int tree_insert_left (TREE_NODE* mom, TREE_NODE* left);
int tree_insert_right (TREE_NODE* mom, TREE_NODE* right);
int tree_remove (TREE_NODE*);
int tree_do (TREE_NODE*, void* param, int (*func)(TREE_NODE*, void*));
int tree_init(TREE_NODE *t);
#endif /* TREE_H_ */
