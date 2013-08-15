/*
 * queue_babs.h
 *  Burst arrival, burst service queue
 *  Created on: Aug 6, 2013
 *      Author: iizke
 */

#ifndef QUEUE_BABS_H_
#define QUEUE_BABS_H_

#include "queues/measures.h"
#include "queues/queue_man.h"
#include "event.h"
#include "netsim.h"
#include "conf/config.h"
#include "queues/packet.h"

typedef struct babs_config {
  /// Configuration of arrival flow
  RANDOM_CONF arrival_conf;
  /// Burst arrival configuration
  RANDOM_CONF burst_conf;
  /// flow configuration of service time
  RANDOM_CONF service_conf;
  /// Configuration of queue system
  QUEUE_CONF queue_conf;
  /// Configuration of terminated conditions
  STOP_CONF stop_conf;
  /// Configuration of random library (IRAND, RANDLIB)
  int random_lib;
  /// protocol: ONE_QUEUE or CSMA
  int protocol;
  /// number of threads
  int nthreads;
  /**
   * Current simulation: used for handling signal SIGINT (we dont want to wait
   * to long time, but also want to see the intermediate result
   */
  void *runtime_state;
} BABSQ_CONFIG;

/**
 * Structure representing the BABS-Queue state in simulation.
 * Similar to one-queue state
 */
typedef struct queue_babs_state {
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
  /// Free packet/request list (used to avoiding malloc operations
  PACKET_LIST free_packets;
  /// Burst generation trace
  STAT_NUM burst_trace;
} BABSQ_STATE;

#define get_babs_state_from_ops(_ops) (container_of(_ops, BABSQ_STATE, ops))

int babs_state_init (BABSQ_STATE *state, BABSQ_CONFIG *conf);
int babs_start (char *conf_file);

#endif /* QUEUE_BABS_H_ */

