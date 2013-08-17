/*
 * burst_sched_window.c
 *  Burst Queue with Window Scheduling
 *
 *  Created on: Aug 17, 2013
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
static int bsw_find_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
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
    packet_list_reset_browsing(&fq->waiting_packets);
    for (; (newp = packet_list_get_next(&fq->waiting_packets)) != NULL;) {
      if (newp->info.burst + total_burst <= fq->max_executing) {
        // choose this packet to go first
        *p = newp;
        packet_list_reset_browsing(&fq->waiting_packets);
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
static int bsw_is_servable (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  PACKET *p = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  bsw_find_waiting_packet(q, &p);
  return (fq->max_executing < 0) ? 1 : ((p) ? 1 : 0);
}

static int bsw_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  *p = NULL;
  fq = (BURST_QINFO*)q->info;
  bsw_find_waiting_packet(q, p);
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
int burst_schedwin_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting) {

  if (! *q_fifo) {
    *q_fifo = malloc_gc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  memset(*q_fifo, 0, sizeof(QUEUE_TYPE));
  burst_schedwin_setup(*q_fifo, max_executing, max_waiting);
  return SUCCESS;
}

/**
 * Setup parameters of a Burst queue (schedwin strategy)
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_schedwin_setup (QUEUE_TYPE *q, int max_executing, int max_waiting) {
  check_null_pointer(q);
  // Inheritance of burst-fifo
  burst_fifo_setup(q, max_executing, max_waiting);
  q->select_waiting_packet = bsw_select_waiting_packet;
  q->is_servable = bsw_is_servable;
  return SUCCESS;
}
