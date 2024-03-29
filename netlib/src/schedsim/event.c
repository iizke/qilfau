/**
 * @file event.c
 * Event and Event-list structure and related functions
 *
 * @date Created on: Oct 7, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "error.h"
#include "event.h"

/**
 * Initialize parametters in SEVENT_LIST
 * @param el : pointer to SEVENT_LIST
 * @return Error-code (normal SUCCESS)
 */
int sevent_list_init (SEVENT_LIST *el, int num_types) {
  check_null_pointer(el);
  try ( linked_list_man_init(&el->list) );
  sem_init(&el->mutex, 0, 1);
  stat_num_init(&el->snum_events);
  el->num_events = 0;
  el->process = malloc_gc(sizeof(void*)*num_types);
  return SUCCESS;
}

int sevent_list_reg_processing_func(SEVENT_LIST* list, int type, int (*func)(void*,void*,void*)) {
  check_null_pointer(list);
  list->process[type] = func;
  return SUCCESS;
}

/**
 * New event structure is allocated and init
 * But this new event is not inserted into referenced Event-list
 * @param el : Event-list used for allocating new memory to Event structure
 * @param e : output -> New event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int sevent_list_new_event_info (SEVENT_LIST *el, int size, SEVENT **e){
  LINKED_LIST *l = NULL;
  check_null_pointer(el);
  try (linked_list_man_get_free_entry(&el->list, &l) );
  if (l)
    *e = container_of(l, SEVENT, list_node);
  else
    *e = malloc_gc(size);

  if(! *e)
    return ERR_MALLOC_FAIL;

  sevent_init(*e);
  return SUCCESS;
}

int sevent_list_new_event_mutex (SEVENT_LIST *el, int size, SEVENT **e){
  int err = SUCCESS;
  sem_wait(&el->mutex);
  err = sevent_list_new_event_info(el, size, e);
  sem_post(&el->mutex);
  return err;
}

/**
 * Insert an event into event-list
 * @param el : Event list
 * @param e : event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int sevent_list_insert_event (SEVENT_LIST *el, SEVENT *e) {
  SEVENT *prev_e = e;

  check_null_pointer(el);
  check_null_pointer(e);
  try ( linked_list_man_insert(&el->list, &e->list_node) );
  /// sort from end of list, assume that event_list already sorted before
  while (&e->list_node != el->list.entries.next) {
    prev_e = container_of(e->list_node.prev, SEVENT, list_node);
    if (e->time >= prev_e->time)
      break;
    swap_prev_event(e);
  }
  el->num_events++;
  stat_num_new_sample(&el->snum_events, el->num_events);
  return SUCCESS;
}

int sevent_list_insert_event_mutex (SEVENT_LIST *el, SEVENT *e) {
  int err = SUCCESS;
  sem_wait(&el->mutex);
  err = sevent_list_insert_event(el, e);
  sem_post(&el->mutex);
  return err;
}

/**
 * Remove an event out of event list.
 * Note that, based on Linked-list-manager (LINKED_LIST_MAN), this event may not
 * be freed but holding for future use (new-event).
 * @param el : Event list
 * @param e : removed event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int sevent_list_remove_event (SEVENT_LIST *el, SEVENT *e) {
  check_null_pointer(el);
  check_null_pointer(e);
  try ( linked_list_man_remove(&el->list, &e->list_node) );
  el->num_events--;
  stat_num_new_sample(&el->snum_events, el->num_events);
  return SUCCESS;
}

int sevent_list_remove_event_mutex (SEVENT_LIST *el, SEVENT *e) {
  int err = SUCCESS;
  sem_wait(&el->mutex);
  err = sevent_list_remove_event(el, e);
  sem_post(&el->mutex);
  return err;
}

/**
 * Get the first event in the event-list
 * @param el : Event-list
 * @param e : Output -> Event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int sevent_list_get_first (SEVENT_LIST *el, SEVENT **e) {
  LINKED_LIST *lle = NULL;
  check_null_pointer(el);
  try ( linked_list_man_get_first(&el->list, &lle) );
  *e = container_of(lle, SEVENT, list_node);
  return SUCCESS;
}

int sevent_list_get_first_mutex (SEVENT_LIST *el, SEVENT **e) {
  int err = SUCCESS;
  sem_wait(&el->mutex);
  err = sevent_list_get_first(el, e);
  sem_post(&el->mutex);
  return err;
}

/**
 * Check an Event-list empty or not
 * @param l : event list
 * @return Return negative number if error. Return 1 if event list is empty, and
 * return 0 otherwise.
 */
int sevent_list_is_empty (SEVENT_LIST *l) {
  check_null_pointer(l);
  return (linked_list_is_empty((&l->list.entries)));
}

/**
 * Do not allow to insert new event to list
 * @param l : event list
 * @return Error code (see more in def.h and error.h)
 */
int sevent_list_stop_growing (SEVENT_LIST *l) {
  check_null_pointer(l);
  sem_wait(&l->mutex);
  l->list.conf = 0;
  sem_post(&l->mutex);
  return SUCCESS;
}

/**
 * Initialize parameters in Event structure
 * @param e : Event
 * @return Error-code (defined in def.h and libs/error.h)
 */
int sevent_init (SEVENT *e) {
  check_null_pointer(e);
  linked_list_init(&e->list_node);
  //e->data = NULL;
  //e->info.action = NULL;
  e->time = 0;
  e->type = 0;
  return SUCCESS;
}

/**
 * Print out to screen some info of every event in a list
 * This info includes event-type, event-time
 * @param l : Event list
 * @return Error-code (defined in def.h and libs/error.h)
 */
int print_sevent_list (SEVENT_LIST *l) {
  SEVENT *e = NULL;
  check_null_pointer(l);
  e = container_of(l->list.entries.next, SEVENT, list_node);
  while (&l->list.entries != &e->list_node) {
    printf("Event %d, time %f \n", e->type, e->time);
    e = container_of(e->list_node.next, SEVENT, list_node);
  }
  return SUCCESS;
}

/**
 * Save an event information into file
 * @param e : Event
 * @param f : File pointer
 * @return Error code (see more in file def.h and libs/error.h )
 */
int sevent_save (SEVENT *e, FILE *f) {
  // FILE f should have write-permission
  // format: <time> <event-type> <processing-time>
  check_null_pointer(f);
  fprintf(f, "%f %d 0\n", e->time, e->type);
  return SUCCESS;
}

/**
 * Setup time of event based on its statistical characteristics.
 * @param e : Event
 * @param fc : random configuration
 * @param curr_time : Current time
 * @return Error code (see more in def.h and error.h)
 */
int sevent_setup (SEVENT *e, RANDOM_SCONF *fc, double curr_time) {
  double time = 0;
  int type;
  double etime;
  struct mmpp_params *p = NULL;
  check_null_pointer(e);
  check_null_pointer(fc);

  switch (fc->type) {
  case RANDOM_FILE:
    fscanf(fc->from_file, "%f %d %f", &time, &type, &etime);
    e->time = time;
    iprint(LEVEL_INFO, "Get time from file, value %f \n", e->time);
    break;
//  case RANDOM_UNIFORM:
//    e->info.time.real = curr_time.real + gen_uniform(fc->from, fc->to);
//    break;
  default:
    if (fc->distribution.gen == NULL) {
      //iprint(LEVEL_WARNING, "Flow type is not supported \n");
      return ERR_RANDOM_TYPE_FAIL;
    }
    time = fc->distribution.gen(&fc->distribution);
    if ((fc->type == RANDOM_MMPP) || (fc->type == RANDOM_MMPP)) {
      p = fc->distribution.params;
      e->time = time;
    } else
      e->time = curr_time + time;
    break;
  }
  return SUCCESS;
}

/**
 * Unit test of function event_list_insert
 * @return Error-code (defined in def.h and libs/error.h)
 */
int test_sevent_list_insert () {
  SEVENT_LIST l;
  SEVENT e1, e2, e3, e4, e5;
  sevent_list_init(&l,2);
  sevent_init(&e1);
  sevent_init(&e2);
  sevent_init(&e3);
  sevent_init(&e4);
  sevent_init(&e5);
  e1.time = 5;
  e2.time = 7;
  e3.time = 3;
  e4.time = 6;
  e5.time = 9;
  sevent_list_insert_event(&l, &e1);
  sevent_list_insert_event(&l, &e2);
  sevent_list_insert_event(&l, &e3);
  sevent_list_insert_event(&l, &e4);
  sevent_list_insert_event(&l, &e5);
  print_event_list(&l);
  return SUCCESS;
}
