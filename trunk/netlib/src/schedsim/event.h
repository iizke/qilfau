/**
 * @file event.h
 * Event structure
 *
 * @date Created on: Oct 7, 2011
 * @author iizke
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <stdio.h>
#include <semaphore.h>
#include "queues/packet.h"
#include "conf/config.h"
#include "queues/measures.h"

#ifdef EVENT
#undef EVENT
#endif
/**
 * Event structure
 */
typedef struct event {
  /// Double linked list. This member is used to join in a list of event
  LINKED_LIST list_node;
  /// Event information
  /// Type of event: EVENT_ARRIVAL, EVENT_END_SERVICE,...
  int type;
  /// Time that this event happens
  double time;
  /// Data related to this event (used for end-service event)
  void *data;
} EVENT;

#ifdef EVENT_LIST
#undef EVENT_LIST
#endif
/**
 * Event list structure
 */
typedef struct event_list {
  /// Manager of double linked list of event
  LINKED_LIST_MAN list;
  sem_t mutex;
  STAT_NUM snum_events;
  int num_events;
  int (*gen) (void *,...);
} EVENT_LIST;

/// Arrival event
#define EVENT_ARRIVAL         1
/// End-of-Service event
#define EVENT_END_SERVICE     2

/// swap two adjacent event: this event and prev-event of this event
#define swap_prev_event(e2)                                 \
  {                                                         \
    LINKED_LIST * _l1 = e2->list_node.prev;                 \
    LINKED_LIST * _l2 = &e2->list_node;                     \
    _l1->next = _l2->next;                                  \
    _l2->prev = _l1->prev;                                  \
    _l1->prev = _l2;                                        \
    _l2->next = _l1;                                        \
    _l1->next->prev = _l1;                                  \
    _l2->prev->next = _l2;                                  \
  }

int sevent_init (EVENT *e);
int sevent_save (EVENT *e, FILE *file);
int sevent_setup (EVENT *e, RANDOM_CONF *fc, double curr_time);

int sevent_list_init (EVENT_LIST *el);
int sevent_list_new_event (EVENT_LIST *el, EVENT **e);
int sevent_list_insert_event (EVENT_LIST *el, EVENT *e);
int sevent_list_remove_event (EVENT_LIST *el, EVENT *e);
int sevent_list_get_first (EVENT_LIST *el, EVENT **e);
int sevent_list_new_event_mutex (EVENT_LIST *el, EVENT **e);
int sevent_list_insert_event_mutex (EVENT_LIST *el, EVENT *e);
int sevent_list_remove_event_mutex (EVENT_LIST *el, EVENT *e);
int sevent_list_get_first_mutex (EVENT_LIST *el, EVENT **e);
int sevent_list_is_empty (EVENT_LIST *l);
int sevent_list_stop_growing (EVENT_LIST *l);
int test_sevent_list_insert ();

#endif /* EVENT_H_ */
