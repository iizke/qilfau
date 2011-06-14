/**
 * @file netqueue.h
 * A network of queues
 *
 * @date Created on: Jun 12, 2011
 * @author iizke
 */

#ifndef NETQUEUE_H_
#define NETQUEUE_H_

#include "queues/measures.h"
#include "queues/queue_man.h"
#include "graph/graph.h"
#include "netsim.h"
#include "event.h"
#include "sys_aqueue.h"

typedef struct netqueue_state {
  /// Abstract system operations
  SYS_STATE_OPS ops;
  /// Current time
  TIME curr_time;
  /// List of future events (used for scheduling events)
  EVENT_LIST future_events;
  /// Queue array (support a list of queues)
  ARRAY queues;
  /// Measurement information
  MEASURES measurement;
  /// Free packet list (used to avoiding malloc operations
  PACKET_LIST free_packets;
  /// Network of queue
  GRAPH queuenet;
} NETQ_STATE;

typedef struct netqueue_one_state {
  /// Queue system state
  ONEQ_STATE state;
  /// Network of queue: global view
  GRAPH *queuenet;
} NETQ_ONE_STATE;

typedef struct netqueue_all_state {
  /// Current global time
  TIME curr_time;
  /// Network of queue
  GRAPH queuenet;
  /// Measurement information
  MEASURES measurement;
  /// Free packet list (used to avoiding malloc operations
  PACKET_LIST free_packets;
} NETQ_ALL_STATE;

#define get_netq_state_from_ops(_ops) (container_of(_ops, NETQ_STATE, ops))

int netq_state_init (NETQ_STATE *state, NET_CONFIG *netconf);

#endif /* NETQUEUE_H_ */
