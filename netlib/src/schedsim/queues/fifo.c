/**
 * @file fifo.c
 * FIFO queue implementation.
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "fifo.h"

/**
 * FIFO queue initialization
 * @param q : a FIFO Queue-type
 * @return Error code (more in def.h and error.h)
 */
static int ff_queue_init (SQUEUE_TYPE *q) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  fq = queue_type_get_fifo_queue(q);

  job_list_init(&fq->incomming_packets, LL_CONF_STORE_ENTRY);
  job_list_init(&fq->dropped_packets, 0);
  job_list_init(&fq->waiting_packets, LL_CONF_STORE_ENTRY);
  job_list_init(&fq->executing_packets, LL_CONF_STORE_ENTRY);
  job_list_init(&fq->departure_packets, 0);
  return SUCCESS;
}

/**
 * Check whether there are some free/availabe servers or not.
 * @param q : FIFO queue type
 * @return 1 if this queue is ready to process a packet
 */
static int ff_is_idle (SQUEUE_TYPE *q) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  fq = queue_type_get_fifo_queue(q);
  return  (fq->max_executing < 0) ? 1 :
              ((fq->executing_packets.size < fq->max_executing) ? 1 : 0);
}

/**
 * Push new packet to the end of queue.
 * @param q : FIFO queue type
 * @param p : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_push_packet (SQUEUE_TYPE *q, JOB *p) {
  int curr_qlen;
  QUEUE_FF *fq = NULL;
  check_null_pointer(p);
  check_null_pointer(q);
  p->state = JOB_STATE_IN;
  p->queue = q;
  smeasurement_self_collect_data(p);
  fq = queue_type_get_fifo_queue(q);
  curr_qlen = fq->waiting_packets.size;
  if ((fq->max_waiting <= curr_qlen) && (fq->max_waiting >= 0)) {
    try ( job_list_insert_job(&fq->dropped_packets, p) );
    p->state = JOB_STATE_DROPPED;
    iprint(LEVEL_INFO, "Packet is dropped \n");
  } else {
    try ( job_list_insert_job(&fq->waiting_packets, p) );
    p->state = JOB_STATE_WAITING;
  }
  smeasurement_self_collect_data(p);
  return SUCCESS;
}

/**
 * Process a packet in queue
 * @param q : FIFO queue
 * @param packet : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_process_packet (SQUEUE_TYPE *q, JOB *packet) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(packet);
  fq = queue_type_get_fifo_queue(q);
  packet->state = JOB_STATE_PROCESSING;
  job_list_insert_job(&fq->executing_packets, packet);
  smeasurement_self_collect_data(packet);
  return SUCCESS;
}

/**
 * Post processing packet when packet is already gone out of queue
 * @param q : FIFO queue type
 * @param packet : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_finish_packet (SQUEUE_TYPE *q, JOB *packet) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  fq = queue_type_get_fifo_queue(q);

  packet->state = JOB_STATE_OUT;
  try ( job_list_remove_job(&fq->executing_packets, packet) );
  try ( job_list_insert_job(&fq->departure_packets, packet) );
  smeasurement_self_collect_data(packet);
  return SUCCESS;
}

/**
 * Determine a number of waiting packets in queue
 * @param q : FIFO queue
 * @return A number of waiting packets in queue
 */
static int ff_get_waiting_length (SQUEUE_TYPE *q) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  fq = queue_type_get_fifo_queue(q);

  return fq->waiting_packets.size;
}

/**
 * Get the first waiting packet and move it out of waiting list
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
static int ff_select_waiting_packet (SQUEUE_TYPE* q, JOB ** p) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = queue_type_get_fifo_queue(q);
  try ( job_list_get_first(&fq->waiting_packets, p) );
  job_list_remove_job(&fq->waiting_packets, *p);
  return SUCCESS;
}

/**
 * Get the first packet in executing list.
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_get_executing_packet (SQUEUE_TYPE* q, JOB ** p) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = queue_type_get_fifo_queue(q);
  try ( job_list_get_first(&fq->executing_packets, p) );
  return SUCCESS;
}

//static JOB * ff_find_executing_packet_to(SQUEUE_TYPE* q, int id) {
//  QUEUE_FF *fq = NULL;
//  LINKED_LIST *lm = NULL, *l = NULL;
//
//  if (!q)
//    return NULL;
//
//  fq = (QUEUE_FF*)q->info;
//  lm = &fq->executing_packets.list.entries;
//  l = lm->next;
//  while (lm != l) {
//    JOB *p = container_of(l, JOB, list_node);
//    if (p->to_queue == id)
//      return p;
//    l = l->next;
//  }
//
//  return NULL;
//}

/**
 * Get the first packet in waiting list, this packet is still in this list
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_get_waiting_packet (SQUEUE_TYPE* q, JOB ** p) {
  QUEUE_FF *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = queue_type_get_fifo_queue(q);
  try ( job_list_get_first(&fq->waiting_packets, p) );
  return SUCCESS;
}

/**
 * Initialization of FIFO queue, maybe allocate new memory if needed
 * @param q_fifo : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int sched_fifo_init (SQUEUE_TYPE **q_fifo, int max_executing, int max_waiting) {
  QUEUE_FF *qf = NULL;
  if (! *q_fifo) {
    qf = malloc_gc(sizeof(QUEUE_FF));
    if (! qf)
      return ERR_MALLOC_FAIL;
    (*q_fifo) = &qf->queue_type;
  }
  memset(*q_fifo, 0, sizeof(SQUEUE_TYPE));
  sched_fifo_setup(*q_fifo, max_executing, max_waiting);
  return SUCCESS;
}

/**
 * Setup parameters of a FIFO queue
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int sched_fifo_setup (SQUEUE_TYPE *q_fifo, int max_executing, int max_waiting) {
  QUEUE_FF *ff_queue_info = NULL;
  check_null_pointer(q_fifo);
  ff_queue_info = queue_type_get_fifo_queue(q_fifo);

  ff_queue_info->max_executing = max_executing;
  ff_queue_info->max_waiting = max_waiting;
  q_fifo->type = QUEUE_FIFO;
  q_fifo->init = ff_queue_init;
  q_fifo->is_idle = ff_is_idle;
  q_fifo->process_packet = ff_process_packet;
  q_fifo->push_packet = ff_push_packet;
  q_fifo->finish_packet = ff_finish_packet;
  q_fifo->get_waiting_length = ff_get_waiting_length;
  q_fifo->select_waiting_packet = ff_select_waiting_packet;
  q_fifo->get_executing_packet = ff_get_executing_packet;
  q_fifo->get_waiting_packet = ff_get_waiting_packet;
  //q_fifo->find_executing_packet_to = ff_find_executing_packet_to;
  q_fifo->init(q_fifo);
  return SUCCESS;
}
