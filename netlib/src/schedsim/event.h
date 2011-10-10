/**
 * @file event.h
 * Event structure
 *
 * @date Created on: Oct 7, 2011
 * @author iizke
 */

#ifndef SCHED_EVENT_H_
#define SCHED_EVENT_H_

#include <stdio.h>
#include <semaphore.h>
#include "queues/job.h"
#include "conf/config.h"
#include "queues/measures.h"

/**
 * Event structure
 */
typedef struct sched_event {
  /// Double linked list. This member is used to join in a list of event
  LINKED_LIST list_node;
  /// Event information
  /// Type of event: EVENT_ARRIVAL, EVENT_END_SERVICE,...
  int type;
  /// Time that this event happens
  double time;
  /// Data related to this event (used for end-service event)
  //void *data;
} SEVENT;

typedef struct sevent_info {
  SEVENT event;
  JOB *packet;
} SEVENT_INFO;

#define event_get_event_info(e) (container_of(e, SEVENT_INFO, event))

/**
 * Event list structure
 */
typedef struct sched_event_list {
  /// Manager of double linked list of event
  LINKED_LIST_MAN list;
  sem_t mutex;
  STAT_NUM snum_events;
  int num_events;
  int (*gen) (void *,...);
} SEVENT_LIST;

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

int sevent_init (SEVENT *e);
int sevent_save (SEVENT *e, FILE *file);
int sevent_setup (SEVENT *e, RANDOM_SCONF *fc, double curr_time);

int sevent_list_init (SEVENT_LIST *el);
int sevent_list_new_event (SEVENT_LIST *el, SEVENT **e);
int sevent_list_insert_event (SEVENT_LIST *el, SEVENT *e);
int sevent_list_remove_event (SEVENT_LIST *el, SEVENT *e);
int sevent_list_get_first (SEVENT_LIST *el, SEVENT **e);
int sevent_list_new_event_mutex (SEVENT_LIST *el, SEVENT **e);
int sevent_list_insert_event_mutex (SEVENT_LIST *el, SEVENT *e);
int sevent_list_remove_event_mutex (SEVENT_LIST *el, SEVENT *e);
int sevent_list_get_first_mutex (SEVENT_LIST *el, SEVENT **e);
int sevent_list_is_empty (SEVENT_LIST *l);
int sevent_list_stop_growing (SEVENT_LIST *l);
int test_sevent_list_insert ();

#endif /* EVENT_H_ */
