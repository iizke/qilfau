/**
 * @file sys_aqueue.h
 * Specification of state in system with one queue
 *
 * @date Created on: May 19, 2011
 * @author iizke
 */

#ifndef SYS_AQUEUE_H_
#define SYS_AQUEUE_H_

#include "queues/measures.h"
#include "queues/queue_man.h"
#include "event.h"
#include "netsim.h"

/**
 * Structure representing the system state in simulation.
 */
typedef struct sys_state {
  /// Abstract operations of a simulated system
  SYS_STATE_OPS ops;
  /// Current time
  TIME curr_time;
  /// List of future events (used for scheduling events)
  EVENT_LIST future_events;
  /// Queue manager (support a list of queues)
  QUEUE_MAN queues;
  /// Measurement information
  MEASURES measurement;
  /// Free packet list (used to avoiding malloc operations
  PACKET_LIST free_packets;
} SYS_STATE;

#define get_sys_state_from_ops(_ops) (container_of(_ops, SYS_STATE, ops))

int sys_state_init (SYS_STATE *state, CONFIG *conf);

#endif /* SYS_AQUEUE_H_ */
