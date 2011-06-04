/**
 * @file linked_list.c
 * Double linked list functions
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "../error.h"

int linked_list_init (LINKED_LIST *l) {
  check_null_pointer(l);
  l->next = l;
  l->prev = l;
  return SUCCESS;
}

int linked_list_insert (LINKED_LIST *l, LINKED_LIST *e) {
  LINKED_LIST *last = NULL;
  check_null_pointer(l);
  check_null_pointer(e);
  /* Connect last element in list to e */
  last = l->prev;
  l->prev = e;
  e->next = l;
  e->prev = last;
  last->next = e;
  return SUCCESS;
}

int linked_list_remove (LINKED_LIST *e) {
  LINKED_LIST *next = NULL;
  LINKED_LIST *prev = NULL;
  check_null_pointer(e);
  prev = e->prev;
  next = e->next;
  check_null_pointer(prev);
  check_null_pointer(next);
  prev->next = next;
  next->prev = prev;
  e->next = e;
  e->prev = e;
  return SUCCESS;
}

int linked_list_get_first(LINKED_LIST *l, LINKED_LIST **e) {
  // not check validity
  check_null_pointer(l);
  if (linked_list_is_empty(l)){
    iprint(LEVEL_WARNING, "No element in list\n");
    *e = NULL;
  } else
    *e = l->next;
  return SUCCESS;
}

int print_list (LINKED_LIST *l) {
  check_null_pointer(l);
  while (! linked_list_is_empty(l)) {
    LINKED_LIST *e = NULL;
    linked_list_get_first(l, &e);
    iprint(LEVEL_INFO, "Get one element\n");
    linked_list_remove(e);
    //free(e);
  }
  return SUCCESS;
}

int test_linked_list () {
  LINKED_LIST l;
  LINKED_LIST e1;
  LINKED_LIST e2;
  LINKED_LIST e3;
  LINKED_LIST e4;
  LINKED_LIST e5;
  printf("Test linked list:\n");
  try ( linked_list_init(&l) );
  try ( linked_list_insert(&l, &e1) );
  try ( linked_list_insert(&l, &e2) );
  try ( linked_list_insert(&l, &e3) );
  try ( linked_list_insert(&l, &e4) );
  try ( linked_list_insert(&l, &e5) );

  print_list(&l);
  printf("Now, linked list is empty \n");
  print_list(&l);
  return SUCCESS;
}

int linked_list_man_init (LINKED_LIST_MAN *lm) {
  check_null_pointer(lm);
  try ( linked_list_init(&(lm->entries)) );
  try ( linked_list_init(&(lm->free_entries)) );
  lm->conf = LL_CONF_STORE_FREE | LL_CONF_STORE_ENTRY;
  return SUCCESS;
}

int linked_list_man_init_conf (LINKED_LIST_MAN *lm, int conf) {
  try (linked_list_man_init(lm));
  lm->conf = conf;
  return SUCCESS;
}

int linked_list_man_insert (LINKED_LIST_MAN *lm, LINKED_LIST *e) {
  check_null_pointer(lm);
  check_null_pointer(e);
  linked_list_remove(e);
  if (lm->conf & LL_CONF_STORE_ENTRY) {
    try ( linked_list_insert(&lm->entries, e) );
  }
  return SUCCESS;
}

int linked_list_man_remove (LINKED_LIST_MAN *lm, LINKED_LIST *e) {
  check_null_pointer(lm);
  check_null_pointer(e);
  try ( linked_list_remove(e) );
  if (lm->conf & LL_CONF_STORE_FREE) {
    try ( linked_list_insert(&lm->free_entries, e) );
  }
  return SUCCESS;
}

int linked_list_man_get_first(LINKED_LIST_MAN *lm, LINKED_LIST **e) {
  check_null_pointer(lm);
  linked_list_get_first(&lm->entries, e);
  return SUCCESS;
}

int linked_list_man_get_free_entry (LINKED_LIST_MAN *lm, LINKED_LIST **e) {
  check_null_pointer(lm);
  if (! linked_list_is_empty( (&lm->free_entries) ) ) {
    try ( linked_list_get_first((&lm->free_entries), e) );
    try ( linked_list_remove(*e) );
  } else {
    *e = NULL;
  }
  return SUCCESS;
}

int linked_list_man_alloc (LINKED_LIST_MAN *lm, LINKED_LIST **e, int size) {
  check_null_pointer(lm);
  try (linked_list_man_get_free_entry(lm, e));
  if (! *e) {
    // allocate new memory
    *e = (LINKED_LIST*) malloc(size);
    if (! *e) {
      iprint(LEVEL_WARNING, "Cannot allocate new memory\n");
      return ERR_LL_MAN_ALLOC;
    }
  }
  return SUCCESS;
}

int test_linked_list_man () {
  LINKED_LIST_MAN lm;
  LINKED_LIST e1;
  LINKED_LIST e2;
  LINKED_LIST e3;
  LINKED_LIST e4;
  LINKED_LIST *e5 = NULL;

  linked_list_man_init(&lm);
  linked_list_man_insert(&lm, &e1);
  linked_list_man_insert(&lm, &e2);
  linked_list_man_insert(&lm, &e3);
  linked_list_man_insert(&lm, &e4);
  linked_list_man_remove(&lm, &e1);
  linked_list_man_alloc(&lm, &e5, sizeof(LINKED_LIST));
  return SUCCESS;
}
