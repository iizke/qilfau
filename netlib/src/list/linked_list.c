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

/**
 * Initialize linked list structure
 * @param l : Linked list
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_init (LINKED_LIST *l) {
  check_null_pointer(l);
  l->next = l;
  l->prev = l;
  l->browsing_at = l;
  return SUCCESS;
}

/**
 * Insert new element into a linked list. Element can be seen as a linked list
 * @param l : Linked list
 * @param e : New element
 * @return Error code (see more in def.h and error.h)
 */
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

int linked_list_insert_head (LINKED_LIST *l, LINKED_LIST *e) {
  LINKED_LIST *head = NULL;
  check_null_pointer(l);
  check_null_pointer(e);
  /* Connect last element in list to e */
  head = l->next;
  l->next = e;
  e->prev = l;
  e->next = head;
  head->prev = e;
  return SUCCESS;
}
/**
 * Remove an element out of linked list
 * @param e : Removed element
 * @return Error code (see more in def.h and error.h)
 */
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

/**
 * Return the first element in a linked list
 * @param l : Linked list
 * @param e : returned the first element in list
 * @return Error code (see more in def.h and error.h)
 */
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

int linked_list_reset_browsing(LINKED_LIST* l) {
  l->browsing_at = l;
  return SUCCESS;
}
LINKED_LIST * linked_list_get_next(LINKED_LIST* l) {
  LINKED_LIST *next = NULL;
  if (!l) return NULL;
  if (! l->browsing_at) linked_list_reset_browsing(l);
  next = l->browsing_at->next;
  if (next == l) return NULL; // end list
  l->browsing_at = next;
  return next;
}

/**
 * Print out all elements of a linked list. Normally used in testing
 * @param l : Linked list
 * @return Error code (see more in def.h and error.h)
 */
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
  LINKED_LIST *next = NULL;
  printf("Test linked list:\n");
  try ( linked_list_init(&l) );
  try ( linked_list_insert(&l, &e1) );
  try ( linked_list_insert(&l, &e2) );
  try ( linked_list_insert(&l, &e3) );
  try ( linked_list_insert(&l, &e4) );
  try ( linked_list_insert(&l, &e5) );

  linked_list_reset_browsing(&l);
  for (; (next = linked_list_get_next(&l) != NULL);) {
    printf("Get one element\n");
  }
  printf("Browsing second time\n");
  for (; (next = linked_list_get_next(&l) != NULL);) {
    printf("Get one element\n");
  }
  printf("Browsing third time\n");
  linked_list_reset_browsing(&l);
  for (; (next = linked_list_get_next(&l) != NULL);) {
    printf("Get one element\n");
  }
  print_list(&l);
  printf("Now, linked list is empty \n");
  print_list(&l);
  return SUCCESS;
}

/**
 * Initialize a manager of linked-list
 * @param lm : linked list manager
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_init (LINKED_LIST_MAN *lm) {
  check_null_pointer(lm);
  try ( linked_list_init(&(lm->entries)) );
  try ( linked_list_init(&(lm->free_entries)) );
  lm->conf = LL_CONF_STORE_FREE | LL_CONF_STORE_ENTRY;
  return SUCCESS;
}

/**
 * Initialize linked list manager with configuration
 * @param lm : Linked list manager
 * @param conf : Configuration. It can be LL_CONF_STORE_FREE or LL_CONF_STORE_ENTRY (or both)
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_init_conf (LINKED_LIST_MAN *lm, int conf) {
  try (linked_list_man_init(lm));
  lm->conf = conf;
  return SUCCESS;
}

/**
 * Insert new element in a Linked list controlled by a manager
 * @param lm : Linked list manager
 * @param e : new element
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_insert (LINKED_LIST_MAN *lm, LINKED_LIST *e) {
  check_null_pointer(lm);
  check_null_pointer(e);
  linked_list_remove(e);
  if (lm->conf & LL_CONF_STORE_ENTRY) {
    try ( linked_list_insert(&lm->entries, e) );
  }
  return SUCCESS;
}

int linked_list_man_insert_head (LINKED_LIST_MAN *lm, LINKED_LIST *e) {
  check_null_pointer(lm);
  check_null_pointer(e);
  linked_list_remove(e);
  if (lm->conf & LL_CONF_STORE_ENTRY) {
    try ( linked_list_insert_head(&lm->entries, e) );
  }
  return SUCCESS;
}

/**
 * Remove an element out of linked list controlled by a manager
 * @param lm : Linked List manager
 * @param e : Removed element. This element is not freed but reused in the next allocation
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_remove (LINKED_LIST_MAN *lm, LINKED_LIST *e) {
  check_null_pointer(lm);
  check_null_pointer(e);
  try ( linked_list_remove(e) );
  if (lm->conf & LL_CONF_STORE_FREE) {
    try ( linked_list_insert(&lm->free_entries, e) );
  }
  return SUCCESS;
}

/**
 * Get the first element in linked list controlled by a manager
 * @param lm : Linked list manager
 * @param e : linked list element
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_get_first(LINKED_LIST_MAN *lm, LINKED_LIST **e) {
  check_null_pointer(lm);
  linked_list_get_first(&lm->entries, e);
  return SUCCESS;
}

/**
 * Get an element which is freed before
 * @param lm : Linked list manager
 * @param e : free element
 * @return Error code (see more in def.h and error.h)
 */
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

/**
 * Allocate a new linked list structure but controlled by a manager. The manager
 * can reused the free memory (last allocation) for this allocation (no need to
 * do malloc).
 * @param lm : Linked list manager
 * @param e : new element
 * @param size : size of new element. This parameter is only used when do malloc.
 * So please take care of using this function: all new structures of linked list
 * have the same size in your program.
 * @return Error code (see more in def.h and error.h)
 */
int linked_list_man_alloc (LINKED_LIST_MAN *lm, LINKED_LIST **e, int size) {
  check_null_pointer(lm);
  try (linked_list_man_get_free_entry(lm, e));
  if (! *e) {
    // allocate new memory
    *e = (LINKED_LIST*) malloc_gc(size);
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
