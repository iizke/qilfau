/**
 * onequeue.h
 * Simulation framework of single queue system
 * 
 * Author: iizke
 * Date: Aug 06 2013
 */

#ifndef ONEQUEUE_H
#define ONEQUEUE_H

#include "netsimv2/des.h"

typedef enum one_queue_event_type {
  E_ARRIVAL = E_END + 1,
  E_END_SERVICE
} Q_EVENT_T;

typedef struct queue_state {
  int (*arrival_process) (EVENT2*, void*);
  int (*service_process) (EVENT2*, void*);
  int (*compose_req) (struct queue_state*);
  int (*start_process) (EVENT2*, void*);
  int (*end_process) (EVENT2*, void*);
  void * data;
} QUEUE_STATE;

int onequeue_start (DES *des, QUEUE_STATE *qs);

#endif
