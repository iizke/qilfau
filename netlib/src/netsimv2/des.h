/*
 * des.h
 * Discrete Event Simulation framework
 *
 *  Created on: Aug 7, 2013
 *      Author: iizke
 */

#ifndef DES_H_
#define DES_H_

#include <stdio.h>
#include <semaphore.h>
#include "../netsim/conf/config.h"
#include "stat_num.h"

// Special events for all systems
typedef enum generic_event_type {
  E_START,
  E_END
} EVENT_T;

typedef struct event2 {
  /// Double linked list. This member is used to join in a list of event
  LINKED_LIST list_node;
  double time;
  int type;
  /// Data input for processing event
  void * data;
} EVENT2;

/**
 * Event list structure
 */
typedef struct event_list2 {
  /// Manager of double linked list of event
  LINKED_LIST_MAN list;
  sem_t mutex;
  STAT_NUM snum_events;
  int num_events;
} EVENT_LIST2;

typedef struct event_control {
  int type;
  int (*process) (EVENT2 * e, void * context);
} EVENT_CTRL;

typedef struct event_control_list {
  int max_slot;
  int max_active_slot;
  EVENT_CTRL list[4];
} EVENT_CTRL_LIST;

typedef struct des {
  /// Event queue
  EVENT_LIST2 eventq;
  /// Processing table
  EVENT_CTRL_LIST control_list;
  /// Current time
  double curtime;
  /// Context
  void *context;
} DES;

typedef int SYS_ID;

int des_setup(DES*);
int event_list2_setup(EVENT_LIST2 *);
int event_ctrl_list_setup(EVENT_CTRL_LIST*);

SYS_ID des_reg_event (DES*, int type, int (*process)(EVENT2*, void*));
int des_reg_context (DES*, void* context);
int des_recv_new_event (DES*, EVENT2*);
int des_run (DES *);
#endif /* DES_H_ */
