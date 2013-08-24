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
#include "optimal/knapsack.h"

int bsw_rearrange_waiting_packet (QUEUE_TYPE *q) {
  int i = 0, count = 0;
  KNAPSACK01_T ks;
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  if (fq->waiting_packets.size < fq->max_window_size)
    fq->window = fq->waiting_packets.size;
  else fq->window = fq->max_window_size;
  // Describe problem using knapsack01 algorithm
  knapsack01_setup(&ks, fq->max_executing - fq->executing_packets.total_burst, fq->window);
  packet_list_reset_browsing(&fq->waiting_packets);
  for (i = 0; i < fq->window; i++) {
    PACKET * p = packet_list_get_next(&fq->waiting_packets);
    if (p) knapsack01_insert_item(&ks, p->info.burst, (p->info.burst));
    else iprint(LEVEL_ERROR,"Null Packet\n");
  }
  packet_list_reset_browsing(&fq->waiting_packets);
  knapsack01_solve2(&ks);
  // rearrage packet followed by knapsack solution
  //packet_list_reset_browsing(&fq->waiting_packets);
  for (i = 0; i < fq->window; i++) {
    PACKET * p = packet_list_get_next(&fq->waiting_packets);
    if ((ks.items)[i].is_chosen == 1 && (ks.items)[i].weight == p->info.burst) {
      // let packet in the first position
      packet_list_move_head(&fq->waiting_packets, p);
      count++;
    } else {
      if ((ks.items)[i].weight != p->info.burst)
        iprint(LEVEL_ERROR, "Mismatch knapsack and waiting list item\n");
    }
  }
//  if (count == 0 && fq->window < fq->max_window_size) fq->window = 0;
//  else
  if (count > 0 && count < fq->window) fq->window = count + 1;
  packet_list_reset_browsing(&fq->waiting_packets);
  gc_free(ks.items);
  return SUCCESS;
}

/**
 * Find packet that optimizing total burst
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
int bsw_find_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  PACKET * newp = NULL;
  long total_burst = 0;
  long burst = 0;
  check_null_pointer(q);
  check_null_pointer(p);
  *p = NULL;
  fq = (BURST_QINFO*)q->info;
  if (fq->max_executing < 0){
    // infinite executing capacity
    *p = newp;
    return SUCCESS;
  }

  bsw_rearrange_waiting_packet(q);

  burst = 0;
  total_burst = fq->executing_packets.total_burst;
  packet_list_get_first(&fq->waiting_packets, &newp);
  if (newp) burst = newp->info.burst;

  if (total_burst + burst <= fq->max_executing) {
    // choose the first request in waiting list
    *p = newp;
  }

  return SUCCESS;
}

/**
 * Check whether there are some free/availabe servers or not.
 * @param q : Burst queue type
 * @return 1 if this queue is ready to process a packet
 */
int bsw_is_servable (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  PACKET *p = NULL;
  long burst = 0;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  bsw_find_waiting_packet(q, &p);
  burst = fq->executing_packets.total_burst;
  if (p) burst += p->info.burst;
  //return (fq->max_executing < 0) ? 1 : ((p != NULL && burst <= fq->max_executing) ? 1 : 0);
  //return (fq->max_executing < 0) ? 1 : ((p != NULL) ? 1 : 0);
  //iprint(LEVEL_ERROR, "servable: max %d, pburst %d p %d\n", fq->max_executing, burst, p);
  if (fq->max_executing < 0) return 1;
  if (p) return 1;
  return 0;
}

int bsw_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  *p = NULL;
  fq = (BURST_QINFO*)q->info;
  bsw_find_waiting_packet(q, p);
  if ((*p) != NULL) {
    packet_list_remove_packet(&fq->waiting_packets, (*p));
    //fq->window--;
  }
  return SUCCESS;
}

/**
 * Initialization of FIFO queue, maybe allocate new memory if needed
 * @param q_fifo : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_schedwin_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting, int window) {

  if (! *q_fifo) {
    *q_fifo = malloc_gc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  memset(*q_fifo, 0, sizeof(QUEUE_TYPE));
  burst_schedwin_setup(*q_fifo, max_executing, max_waiting, window);
  return SUCCESS;
}

/**
 * Setup parameters of a Burst queue (schedwin strategy)
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_schedwin_setup (QUEUE_TYPE *q, int max_executing, int max_waiting, int window) {
  BURST_QINFO *queue_info = NULL;
  check_null_pointer(q);
  // Inheritance of burst-fifo
  burst_fifo_setup(q, max_executing, max_waiting);
  queue_info = (BURST_QINFO*)q->info;
  queue_info->max_window_size = window;
  q->type = QUEUE_BURST_SCHEDWIN;
  q->select_waiting_packet = bsw_select_waiting_packet;
  q->is_servable = bsw_is_servable;
  return SUCCESS;
}
