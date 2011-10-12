/**
 * @file sys_aqueue.h
 * Specification of state in system with one queue
 *
 * @date Created on: May 19, 2011
 * @author iizke
 */

#ifndef SCHEDSYS_AQUEUE_H_
#define SCHEDSYS_AQUEUE_H_

#include "queues/measures.h"
#include "queues/queue_man.h"
#include "event.h"
#include "schedsim.h"
#include "conf/config.h"

/**
 * Structure representing the system state in simulation.
 */
typedef struct onequeue_state {
  /// Abstract operations of a simulated system
  SCHED_STATE_OPS ops;
  /// Current time
  double curr_time;
  /// List of future events (used for scheduling events)
  SEVENT_LIST future_events;
  /// Queue manager (support a list of queues)
  SQUEUE_MAN queues;
  /// Measurement information
  SCHED_MEASURES measurement;
  /// Free packet list (used to avoiding malloc operations
  JOB_LIST free_packets;
} ONEQ_STATE;

#define get_oneq_state_from_ops(_ops) (container_of(_ops, ONEQ_STATE, ops))

typedef struct sevent_oneq {
  SEVENT event;
  JOB *packet;
//  ONEQ_STATE *state;
//  RANDOM_SCONF *conf;
} SEVENT_ONEQ;

#define event_oneq_from_event(e) (container_of(e, SEVENT_ONEQ, event))

int sched_sys_state_init (ONEQ_STATE *state, SCHED_CONFIG *conf);
int oneq_set_ops (ONEQ_STATE *state) ;

int oneq_new_packet (ONEQ_STATE *state, JOB **p);
int oneq_free_packet (ONEQ_STATE *state, JOB *p);
int oneq_update_time (ONEQ_STATE *state, double time);

#endif /* SYS_AQUEUE_H_ */
