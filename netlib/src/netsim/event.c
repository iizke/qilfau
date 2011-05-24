/**
 * @file event.c
 * Event and Event-list structure and related functions
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "../error.h"
#include "event.h"

/**
 * Initialize parametters in EVENT_LIST
 * @param el : pointer to EVENT_LIST
 * @return Error-code (normal SUCCESS)
 */
int event_list_init (EVENT_LIST *el) {
  check_null_pointer(el);
  try ( linked_list_man_init(&el->list) );
  return SUCCESS;
}

/**
 * New event structure is allocated and init
 * But this new event is not inserted into referenced Event-list
 * @param el : Event-list used for allocating new memory to Event structure
 * @param e : output -> New event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int event_list_new_event (EVENT_LIST *el, EVENT **e){
  LINKED_LIST *l = NULL;
  check_null_pointer(el);
  try (linked_list_man_get_free_entry(&el->list, &l) );
  if (l)
    *e = container_of(l, EVENT, list_node);
  else
    *e = malloc(sizeof(EVENT));
  if(! *e)
    return ERR_MALLOC_FAIL;

  event_init(*e);
  return SUCCESS;
}

/**
 * Insert an event into event-list
 * @param el : Event list
 * @param e : event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int event_list_insert_event (EVENT_LIST *el, EVENT *e) {
  EVENT *prev_e = e;

  check_null_pointer(el);
  check_null_pointer(e);
  try ( linked_list_man_insert(&el->list, &e->list_node) );
  /// sort from end of list, assume that event_list already sorted before
  while (&e->list_node != el->list.entries.next) {
    prev_e = container_of(e->list_node.prev, EVENT, list_node);
    if (e->info.time.real >= prev_e->info.time.real)
      break;
    swap_prev_event(e);
  }

  return SUCCESS;
}

/**
 * Remove an event out of event list.
 * Note that, based on Linked-list-manager (LINKED_LIST_MAN), this event may not
 * be freed but holding for future use (new-event).
 * @param el : Event list
 * @param e : removed event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int event_list_remove_event (EVENT_LIST *el, EVENT *e) {
  check_null_pointer(el);
  check_null_pointer(e);
  try ( linked_list_man_remove(&el->list, &e->list_node) );
  return SUCCESS;
}

/**
 * Get the first event in the event-list
 * @param el : Event-list
 * @param e : Output -> Event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int event_list_get_first (EVENT_LIST *el, EVENT **e) {
  LINKED_LIST *lle = NULL;
  check_null_pointer(el);
  try ( linked_list_man_get_first(&el->list, &lle) );
  *e = container_of(lle, EVENT, list_node);
  return SUCCESS;
}

/**
 * Check an Event-list empty or not
 * @param l : event list
 * @return Return negative number if error. Return 1 if event list is empty, and
 * return 0 otherwise.
 */
int event_list_is_empty (EVENT_LIST *l) {
  check_null_pointer(l);
  return (linked_list_is_empty((&l->list.entries)));
}

/**
 * Initialize parameters in Event structure
 * @param e : Event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int event_init (EVENT *e) {
  check_null_pointer(e);
  linked_list_init(&e->list_node);
  e->info.packet = NULL;
  //e->info.action = NULL;
  e->info.time.real = 0;
  e->info.type = 0;
  return SUCCESS;
}

/**
 * Print out to screen some info of every event in a list
 * This info includes event-type, event-time
 * @param l : Event list
 * @return Error-code (defined in def.h and libs/error.h)
 */
int print_event_list (EVENT_LIST *l) {
  EVENT *e = NULL;
  check_null_pointer(l);
  e = container_of(l->list.entries.next, EVENT, list_node);
  while (&l->list.entries != &e->list_node) {
    printf("Event %d, time %f \n", e->info.type, e->info.time.real);
    e = container_of(e->list_node.next, EVENT, list_node);
  }
  return SUCCESS;
}

/**
 * Save an event information into file
 * @param e : Event
 * @param f : File pointer
 * @return Error code (see more in file def.h and libs/error.h )
 */
int event_save (EVENT *e, FILE *f) {
  // FILE f should have write-permission
  // format: <time> <event-type> <processing-time>
  check_null_pointer(f);
  fprintf(f, "%f %d 0\n", e->info.time.real, e->info.type);
  return SUCCESS;
}

/**
 * Unit test of function event_list_insert
 * @return Error-code (defined in def.h and libs/error.h)
 */
int test_event_list_insert () {
  EVENT_LIST l;
  EVENT e1, e2, e3, e4, e5;
  event_list_init(&l);
  event_init(&e1);
  event_init(&e2);
  event_init(&e3);
  event_init(&e4);
  event_init(&e5);
  e1.info.time.real = 5;
  e2.info.time.real = 7;
  e3.info.time.real = 3;
  e4.info.time.real = 6;
  e5.info.time.real = 9;
  event_list_insert_event(&l, &e1);
  event_list_insert_event(&l, &e2);
  event_list_insert_event(&l, &e3);
  event_list_insert_event(&l, &e4);
  event_list_insert_event(&l, &e5);
  print_event_list(&l);
  return SUCCESS;
}
