/**
 * @file csma.h
 * CSMA system structure
 *
 * @date Created on: May 20, 2011
 * @author iizke
 */

#ifndef CSMA_H_
#define CSMA_H_

#include "event.h"
#include "../queues/measures.h"
#include "netsim.h"

typedef struct csma_state {
  /// Abstract system operations
  SYS_STATE_OPS ops;
  /// Channel state
  int channel_state;
  /// Current time
  TIME curr_time;
  /// List of future events (used for scheduling events)
  EVENT_LIST future_events;
  /// Queue manager (support a list of queues)
  QUEUE_MAN *queues;
  /// Number of queues
  int nqueues;
  /// Measurement information
  MEASURES measurement;
  /// Free packet list (used to avoiding malloc operations
  PACKET_LIST free_packets;
} CSMA_STATE;

/// Channel state is Busy
#define CHANNEL_BUSY              1
/// Channel state is Free
#define CHANNEL_FREE              0

/// Queue state is Non-persistent CSMA
#define QUEUE_STATE_NOPERSISTENT  0
/// Queue state is persistent CSMA
#define QUEUE_STATE_PERSISTENT    1
/// Queue is transfering packet
#define QUEUE_STATE_TRANSFERING   2

int csma_state_init (CSMA_STATE *state, CONFIG *conf);

#endif /* CSMA_H_ */
