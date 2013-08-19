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
typedef struct burst_queue {
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
  /// Find the optimal solution within a Window size
  int window;
  int max_window_size;
  /// Maximum number of executing packets
  long max_executing;
  /// Maximum number of waiting packets
  long max_waiting;
} BURST_QINFO;



int burst_fifo_init (QUEUE_TYPE **q, int max_executing, int max_waiting);
int burst_fifo_setup (QUEUE_TYPE *q, int max_executing, int max_waiting);

int burst_sched1_init (QUEUE_TYPE **q, int max_executing, int max_waiting, int window);
int burst_sched1_setup (QUEUE_TYPE *q, int max_executing, int max_waiting, int window);

int burst_schedwin_init (QUEUE_TYPE **q, int max_executing, int max_waiting, int window);
int burst_schedwin_setup (QUEUE_TYPE *q, int max_executing, int max_waiting, int window);

#endif /* BURST_FIFO_H_ */
