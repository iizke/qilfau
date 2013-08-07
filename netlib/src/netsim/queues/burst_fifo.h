/*
 * burst_fifo.h
 *
 *  Created on: Aug 7, 2013
 *      Author: iizke
 */

#ifndef BURST_FIFO_H_
#define BURST_FIFO_H_

#include "queue_man.h"
#include "measures.h"

/**
 * FIFO queue structure
 */
typedef struct burst_fifo_queue {
  /// Incomming packet list
  PACKET_LIST incomming_packets;
  /// Waiting packet list
  PACKET_LIST waiting_packets;
  /// Executing packet list
  PACKET_LIST executing_packets;
  /// Dropped packet list
  PACKET_LIST dropped_packets;
  /// Departure packet list
  PACKET_LIST departure_packets;
  /// Queue measurement
  MEASURES measurement;
  /// Queue state
  int state;
  /// Maximum number of executing packets
  int max_executing;
  /// Maximum number of waiting packets
  int max_waiting;
} BURST_FIFO_QINFO;

int burst_fifo_init (QUEUE_TYPE **q, int max_executing, int max_waiting);
int burst_fifo_setup (QUEUE_TYPE *q, int max_executing, int max_waiting);


#endif /* BURST_FIFO_H_ */
