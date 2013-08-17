/*
 * burst_fifo_sched.c
 *  BFFS: Burst FIFO queue with Scheduling
 *  Created on: Aug 7, 2013
 *      Author: iizke
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "burst_fifo.h"

/**
 * Get the first waiting packet and move it out of waiting list
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
static int bffs_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (BURST_FIFO_QINFO*)q->info;
  try ( packet_list_get_first(&fq->waiting_packets, p) );
  packet_list_remove_packet(&fq->waiting_packets, *p);
  return SUCCESS;
}

/**
 * Get the first packet in waiting list, this packet is still in this list
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int bffs_get_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (BURST_FIFO_QINFO*)q->info;
  try ( packet_list_get_first(&fq->waiting_packets, p) );
  return SUCCESS;
}

/**
 * Initialization of FIFO queue, maybe allocate new memory if needed
 * @param q_fifo : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_fifo_sched_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting) {

  if (! *q_fifo) {
    *q_fifo = malloc_gc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  memset(*q_fifo, 0, sizeof(QUEUE_TYPE));
  burst_fifo_sched_setup(*q_fifo, max_executing, max_waiting);
  return SUCCESS;
}

/**
 * Setup parameters of a FIFO queue
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_fifo_sched_setup (QUEUE_TYPE *q_fifo, int max_executing, int max_waiting) {

  check_null_pointer(q_fifo);

  burst_fifo_setup(q_fifo, max_executing, max_waiting);

  q_fifo->select_waiting_packet = bffs_select_waiting_packet;
  q_fifo->get_waiting_packet = bffs_get_waiting_packet;
  return SUCCESS;
}
