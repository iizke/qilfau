/*
 * burst_fifo_sched.c
 *  BFFS: Burst queue with First Fit Scheduling
 *  Created on: Aug 16, 2013
 *      Author: iizke
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "burst_queue.h"

/**
 * If the first request in the waiting list could not be served,
 * find other reasonable request in this list
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
static int bs1_find_firstfit_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  PACKET * newp = NULL;
  long total_burst = 0;
  long burst = 0;
  check_null_pointer(q);
  check_null_pointer(p);
  *p = NULL;
  fq = (BURST_QINFO*)q->info;
  packet_list_get_first(&fq->waiting_packets, &newp);
  total_burst = fq->executing_packets.total_burst;
  if (newp) burst = newp->info.burst;
  if (fq->max_executing < 0){
    // infinite executing capacity
    *p = newp;
    return SUCCESS;
  }

  if (total_burst + burst <= fq->max_executing) {
    // choose the first request in waiting list
    *p = newp;
  } else {
    // find reasonable requests
    int count = 0;
    packet_list_reset_browsing(&fq->waiting_packets);
    for (; (newp = packet_list_get_next(&fq->waiting_packets)) != NULL;) {
      if (newp->info.burst + total_burst <= fq->max_executing) {
        // choose this packet to go first
        *p = newp;
        packet_list_reset_browsing(&fq->waiting_packets);
        break;
      }
      if (fq->max_window_size > 0) {
        count++;
        if (count >= fq->max_window_size)
          break;
      }
    }
  }
  return SUCCESS;
}

/**
 * Check whether there are some free/availabe servers or not.
 * @param q : Burst queue type
 * @return 1 if this queue is ready to process a packet
 */
static int bs1_is_servable (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  PACKET *p = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  bs1_find_firstfit_waiting_packet(q, &p);
  return (fq->max_executing < 0) ? 1 : ((p) ? 1 : 0);
}

static int bs1_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  *p = NULL;
  fq = (BURST_QINFO*)q->info;
  bs1_find_firstfit_waiting_packet(q, p);
  packet_list_remove_packet(&fq->waiting_packets, *p);
  return SUCCESS;
}

/**
 * Initialization of FIFO queue, maybe allocate new memory if needed
 * @param q_fifo : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_sched1_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting, int window) {

  if (! *q_fifo) {
    *q_fifo = malloc_gc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  memset(*q_fifo, 0, sizeof(QUEUE_TYPE));
  burst_sched1_setup(*q_fifo, max_executing, max_waiting, window);
  return SUCCESS;
}

/**
 * Setup parameters of a FIFO queue
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_sched1_setup (QUEUE_TYPE *q, int max_executing, int max_waiting, int window) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  burst_fifo_setup(q, max_executing, max_waiting);
  fq = (BURST_QINFO *)q->info;
  q->type = QUEUE_BURST_SCHED1;
  fq->max_window_size = window;
  q->select_waiting_packet = bs1_select_waiting_packet;
  q->is_servable = bs1_is_servable;
  return SUCCESS;
}

