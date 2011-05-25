/**
 * @file fifo.c
 * FIFO queue implementation.
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <stdio.h>
#include "../error.h"
#include "fifo.h"

/**
 * FIFO queue initialization
 * @param q : a FIFO Queue-type
 * @return Error code (more in def.h and error.h)
 */
static int ff_queue_init (QUEUE_TYPE *q) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (FIFO_QINFO*)q->info;

  packet_list_init(&fq->incomming_packets, LL_CONF_STORE_ENTRY);
  packet_list_init(&fq->dropped_packets, 0);
  packet_list_init(&fq->waiting_packets, LL_CONF_STORE_ENTRY);
  packet_list_init(&fq->executing_packets, LL_CONF_STORE_ENTRY);
  packet_list_init(&fq->departure_packets, 0);
  return SUCCESS;
}

/**
 * Check whether there are some free/availabe servers or not.
 * @param q : FIFO queue type
 * @return 1 if this queue is ready to process a packet
 */
static int ff_is_idle (QUEUE_TYPE *q) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (FIFO_QINFO*)q->info;
  return  (fq->max_executing < 0) ? 1 : (fq->executing_packets.size < fq->max_executing);
}

/**
 * Push new packet to the end of queue.
 * @param q : FIFO queue type
 * @param p : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_push_packet (QUEUE_TYPE *q, PACKET *p) {
  int curr_qlen;
  FIFO_QINFO *fq = NULL;
  check_null_pointer(p);
  check_null_pointer(q);
  p->info.state = PACKET_STATE_IN;
  p->info.queue = q;
  measurement_self_collect_data(p);
  fq = (FIFO_QINFO*)q->info;
  curr_qlen = fq->waiting_packets.size;
  if ((fq->max_waiting <= curr_qlen) && (fq->max_waiting >= 0)) {
    try ( packet_list_insert_packet(&fq->dropped_packets, p) );
    p->info.state = PACKET_STATE_DROPPED;
    iprintf(LEVEL_INFO, "Packet is dropped \n");
  } else {
    try ( packet_list_insert_packet(&fq->waiting_packets, p) );
    p->info.state = PACKET_STATE_WAITING;
  }
  measurement_self_collect_data(p);
  return SUCCESS;
}

/**
 * Process a packet in queue
 * @param q : FIFO queue
 * @param packet : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_process_packet (QUEUE_TYPE *q, PACKET *packet) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(packet);
  fq = (FIFO_QINFO*)q->info;
  packet->info.state = PACKET_STATE_PROCESSING;
  packet_list_insert_packet(&fq->executing_packets, packet);
  measurement_self_collect_data(packet);
  return SUCCESS;
}

/**
 * Post processing packet when packet is already gone out of queue
 * @param q : FIFO queue type
 * @param packet : packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_finish_packet (QUEUE_TYPE *q, PACKET *packet) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (FIFO_QINFO*)q->info;
  packet->info.state = PACKET_STATE_OUT;
  try ( packet_list_remove_packet(&fq->executing_packets, packet) );
  try ( packet_list_insert_packet(&fq->departure_packets, packet) );
  measurement_self_collect_data(packet);
  return SUCCESS;
}

/**
 * Determine a number of waiting packets in queue
 * @param q : FIFO queue
 * @return A number of waiting packets in queue
 */
static int ff_get_waiting_length (QUEUE_TYPE *q) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (FIFO_QINFO*)q->info;
  return fq->waiting_packets.size;
}

/**
 * Get the first waiting packet and move it out of waiting list
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
static int ff_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (FIFO_QINFO*)q->info;
  try ( packet_list_get_first(&fq->waiting_packets, p) );
  packet_list_remove_packet(&fq->waiting_packets, *p);
  return SUCCESS;
}

/**
 * Get the first packet in executing list.
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_get_executing_packet (QUEUE_TYPE* q, PACKET ** p) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (FIFO_QINFO*)q->info;
  try ( packet_list_get_first(&fq->executing_packets, p) );
  return SUCCESS;
}

/**
 * Get the first packet in waiting list, this packet is still in this list
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int ff_get_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  FIFO_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (FIFO_QINFO*)q->info;
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
int fifo_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting) {

  if (! *q_fifo) {
    *q_fifo = malloc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  fifo_setup(*q_fifo, max_executing, max_waiting);
  return SUCCESS;
}

/**
 * Setup parameters of a FIFO queue
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int fifo_setup (QUEUE_TYPE *q_fifo, int max_executing, int max_waiting) {
  FIFO_QINFO *ff_queue_info = NULL;
  check_null_pointer(q_fifo);

  if (!q_fifo->info) {
    ff_queue_info = malloc(sizeof(FIFO_QINFO));
    if (!ff_queue_info)
      return ERR_MALLOC_FAIL;
    q_fifo->info = ff_queue_info;
  }
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

  q_fifo->init(q_fifo);
  return SUCCESS;
}
