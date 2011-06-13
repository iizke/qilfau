/**
 * @file event.h
 * Event structure
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <stdio.h>
#include "queues/packet.h"
#include "conf/config.h"

/**
 * Information in an event
 */
typedef struct event_info {
  /// Type of event: EVENT_ARRIVAL, EVENT_END_SERVICE,...
  int type;
  /// Time that this event happens
  TIME time;
  /// Packet related to this event (used for end-service event)
  PACKET *packet;
} EVENTINFO;

/**
 * Event structure
 */
typedef struct event {
  /// Double linked list. This member is used to join in a list of event
  LINKED_LIST list_node;
  /// Event information
  EVENTINFO info;
  //int (*process) (void *, ...);
} EVENT;

/**
 * Event list structure
 */
typedef struct event_list {
  /// Manager of double linked list of event
  LINKED_LIST_MAN list;
  int (*gen) (void *,...);
} EVENT_LIST;

/// Arrival event
#define EVENT_ARRIVAL         1
/// End-of-Service event
#define EVENT_END_SERVICE     2
/// Access Channel event
#define EVENT_ACCESS_CHANNEL  3
/// End Collision event
#define EVENT_END_COLLISION   4
/// Signal SoT (Start of Transmission) event
#define EVENT_SIGNAL_SOT      5
/// Signal EoT (End of Transmission) event
#define EVENT_SIGNAL_EOT      6


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

int event_init (EVENT *e);
int event_save (EVENT *e, FILE *file);
int event_setup (EVENT *e, RANDOM_CONF *fc, TIME curr_time);

int event_list_init (EVENT_LIST *el);
int event_list_new_event (EVENT_LIST *el, EVENT **e);
int event_list_insert_event (EVENT_LIST *el, EVENT *e);
int event_list_remove_event (EVENT_LIST *el, EVENT *e);
int event_list_get_first (EVENT_LIST *el, EVENT **e);
int event_list_is_empty (EVENT_LIST *l);
int event_list_stop_growing (EVENT_LIST *l);
int test_event_list_insert ();

#endif /* EVENT_H_ */
