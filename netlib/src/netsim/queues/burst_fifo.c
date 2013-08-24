/*
 * burst_fifo.c
 *
 *  Created on: Aug 7, 2013
 *      Author: iizke
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "burst_queue.h"

/**
 * Burst FIFO queue initialization
 * @param q : a FIFO Queue-type
 * @return Error code (more in def.h and error.h)
 */
static int bff_queue_init (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;

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
static int bff_is_servable (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  PACKET *p = NULL;
  int burst = 0;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  packet_list_get_first(&fq->waiting_packets, &p);
  if (p) burst = p->info.burst;
  return  (fq->max_executing < 0) ? 1 : (((fq->executing_packets.total_burst + burst) <= fq->max_executing) ? 1 : 0);
}

static int bff_is_processing (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  return (fq->executing_packets.size + fq->waiting_packets.size > 0);
}

/**
 * Push new packet to the end of queue.
 * @param q : FIFO queue type
 * @param p : packet
 * @return Error code (more in def.h and error.h)
 */
static int bff_push_packet (QUEUE_TYPE *q, PACKET *p) {
  int curr_qlen;
  BURST_QINFO *fq = NULL;
  check_null_pointer(p);
  check_null_pointer(q);
  p->info.state = PACKET_STATE_IN;
  p->info.queue = q;
  measurement_self_collect_data(p);
  fq = (BURST_QINFO*)q->info;
  curr_qlen = fq->waiting_packets.size;
  if ((fq->max_waiting <= curr_qlen) && (fq->max_waiting >= 0)) {
    try ( packet_list_insert_packet(&fq->dropped_packets, p) );
    p->info.state = PACKET_STATE_DROPPED;
    iprint(LEVEL_INFO, "Packet is dropped \n");
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
static int bff_process_packet (QUEUE_TYPE *q, PACKET *packet) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(packet);
  fq = (BURST_QINFO*)q->info;
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
static int bff_finish_packet (QUEUE_TYPE *q, PACKET *packet) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
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
static int bff_get_waiting_length (QUEUE_TYPE *q) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  return fq->waiting_packets.size;
}

/**
 * Get the first waiting packet and move it out of waiting list
 * @param q : FIFO queue
 * @param p : selected packet (output)
 * @return Error code (more in def.h and error.h)
 */
static int bff_select_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (BURST_QINFO*)q->info;
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
static int bff_get_executing_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (BURST_QINFO*)q->info;
  try ( packet_list_get_first(&fq->executing_packets, p) );
  return SUCCESS;
}

static PACKET * bff_find_executing_packet_to(QUEUE_TYPE* q, int id) {
  BURST_QINFO *fq = NULL;
  LINKED_LIST *lm = NULL, *l = NULL;

  if (!q)
    return NULL;

  fq = (BURST_QINFO*)q->info;
  lm = &fq->executing_packets.list.entries;
  l = lm->next;
  while (lm != l) {
    PACKET *p = container_of(l, PACKET, list_node);
    if (p->info.to_queue == id)
      return p;
    l = l->next;
  }

  return NULL;
}

/**
 * Get the first packet in waiting list, this packet is still in this list
 * @param q : FIFO queue
 * @param p : returned packet
 * @return Error code (more in def.h and error.h)
 */
static int bff_get_waiting_packet (QUEUE_TYPE* q, PACKET ** p) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  check_null_pointer(p);
  fq = (BURST_QINFO*)q->info;
  try ( packet_list_get_first(&fq->waiting_packets, p) );
  return SUCCESS;
}

static int bff_get_active_servers (QUEUE_TYPE* q) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  return fq->executing_packets.total_burst;
}

static int bff_get_max_servers (QUEUE_TYPE* q) {
  BURST_QINFO *fq = NULL;
  check_null_pointer(q);
  fq = (BURST_QINFO*)q->info;
  return fq->max_executing;
}


/**
 * Initialization of FIFO queue, maybe allocate new memory if needed
 * @param q_fifo : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_fifo_init (QUEUE_TYPE **q_fifo, int max_executing, int max_waiting) {

  if (! *q_fifo) {
    *q_fifo = malloc_gc(sizeof(QUEUE_TYPE));
    if (! *q_fifo)
      return ERR_MALLOC_FAIL;
  }
  memset(*q_fifo, 0, sizeof(QUEUE_TYPE));
  burst_fifo_setup(*q_fifo, max_executing, max_waiting);
  return SUCCESS;
}

/**
 * Setup parameters of a FIFO queue
 * @param q_fifo : FIFO queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int burst_fifo_setup (QUEUE_TYPE *q_fifo, int max_executing, int max_waiting) {
  BURST_QINFO *bff_queue_info = NULL;
  check_null_pointer(q_fifo);

  if (!q_fifo->info) {
    bff_queue_info = malloc_gc(sizeof(BURST_QINFO));
    if (!bff_queue_info)
      return ERR_MALLOC_FAIL;
    q_fifo->info = bff_queue_info;
  }
  bff_queue_info->max_executing = max_executing;
  bff_queue_info->max_waiting = max_waiting;
  q_fifo->type = QUEUE_BURST_FIFO;
  q_fifo->init = bff_queue_init;
  q_fifo->is_servable = bff_is_servable;
  q_fifo->is_processing = bff_is_processing;
  q_fifo->process_packet = bff_process_packet;
  q_fifo->push_packet = bff_push_packet;
  q_fifo->finish_packet = bff_finish_packet;
  q_fifo->get_waiting_length = bff_get_waiting_length;
  q_fifo->select_waiting_packet = bff_select_waiting_packet;
  q_fifo->get_executing_packet = bff_get_executing_packet;
  q_fifo->get_waiting_packet = bff_get_waiting_packet;
  q_fifo->find_executing_packet_to = bff_find_executing_packet_to;
  q_fifo->get_active_servers = bff_get_active_servers;
  q_fifo->get_max_servers = bff_get_max_servers;
  q_fifo->init(q_fifo);
  return SUCCESS;
}
