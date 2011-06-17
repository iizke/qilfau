/**
 * @file netqueue.h
 * A network of queues
 *
 * @date Created on: Jun 12, 2011
 * @author iizke
 */

#ifndef NETQUEUE_H_
#define NETQUEUE_H_

#include <semaphore.h>
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

#define NETQ_ONE_STATE_OFF        1
#define NETQ_ONE_STATE_ON         2
#define NETQ_ONE_STATE_RUNNING    3
#define NETQ_ONE_STATE_WAITING    4

typedef struct netqueue_one_state {
  /// Queue system state
  ONEQ_STATE qstate;
  /// Network of queue: global view
  GRAPH *queuenet;
  /// Routing to find the next node
  RANDOM_DIST * routing;
  /// Waiting for this node
  int waited_node;
  /// State: ON or OFF (based on exiting conditions) or RUNNING
  int state;
  sem_t mutex;
} NETQ_ONE_STATE;

typedef struct netqueue_all_state {
  /// Current global time
  TIME curr_time;
  /// Network of queue: queues and channels
  GRAPH queuenet;
  /// Queue array (support a list of queueing nodes): structure of NETQ_ONE_STATE
  ARRAY nodes;
  /// Measurement information : black box of all queue
  MEASURES measurement;
//  /// Free packet list (used to avoiding malloc operations
//  PACKET_LIST free_packets;
//  sem_t mutex;
} NETQ_ALL_STATE;

#define get_netq_state_from_ops(_ops) (container_of(_ops, NETQ_STATE, ops))
#define get_netq_one_state_from_ops(_ops) (container_of((get_sys_state_from_ops(_ops)), NETQ_ONE_STATE, qstate))

int netq_state_init (NETQ_STATE *state, NET_CONFIG *netconf);

#endif /* NETQUEUE_H_ */
