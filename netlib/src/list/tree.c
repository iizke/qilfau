/**
 * @file tree.c
 *
 * @date Created on: Aug 29, 2011
 * @author iizke
 */

#include "error.h"
#include "tree.h"

int tree_insert_mom (TREE_NODE* mom, TREE_NODE* left, TREE_NODE* right) {
  check_null_pointer(mom);
  mom->left = left;
  mom->right = right;
  left->mom = mom;
  right->mom = mom;
  return SUCCESS;
}

int tree_insert_left (TREE_NODE* mom, TREE_NODE* left) {
  check_null_pointer(mom);
  mom->left = left;
  left->mom = mom;
  return SUCCESS;
}

int tree_insert_right (TREE_NODE* mom, TREE_NODE* right) {
  check_null_pointer(mom);
  mom->right = right;
  right->mom = mom;
  return SUCCESS;
}

int tree_remove (TREE_NODE* t) {
  TREE_NODE *mom = t->mom;
  if (mom != t) {
    if (mom->left == t)
      mom->left = NULL;
    if (mom->right == t)
      mom->right = NULL;
  }
  return SUCCESS;
}

int tree_do (TREE_NODE* t, void* param, int (*func)(TREE_NODE*, void*)) {
  if (!t)
    return ERR_TREE_NODE_NULL;
  if (t->state == TREE_STATE_WAITED)
    return ERR_TREE_COMPUTING_PENDING;

  if (t->left)
    tree_do(t->left, param, func);
  if (t->right)
    tree_do(t->right, param, func);
  func(t, param);
  return SUCCESS;
}

int tree_init(TREE_NODE *t) {
  t->left = NULL;
  t->right = NULL;
  t->mom = NULL;
  t->state = TREE_STATE_NORMAL;
  return SUCCESS;
}
