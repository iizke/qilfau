/**
 * @file packet.c
 * Packet functions
 *
 * @date Created on: Apr 8, 2011
 * @author iizke
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "packet.h"
#include "fifo.h"

/**
 * Initialization a packet
 * @param p : Packet
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_init (PACKET *p) {
  check_null_pointer(p);
  memset(p, 0, sizeof(PACKET));
  linked_list_init(&p->list_node);
  p->info.burst = 1;
  return SUCCESS;
}

/**
 * Initialization of a packet list
 * @param l : Packet list
 * @param conf : configuration of list
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_init (PACKET_LIST *l, int conf) {
  check_null_pointer(l);
  try ( linked_list_man_init(&l->list) );
  l->size = 0;
  packet_list_reset_browsing(l);
  packet_list_config(l, conf);
  return SUCCESS;
}

/**
 * Create new packet.
 * If there is no free packet in packet-list, new memory is allocated to new packet.
 * Note: if packet list is configured to no used free-packets, always allocate new
 * memory to packet.
 * @param pf : packet list
 * @param p : new packet (output)
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_new_packet (PACKET_LIST *pf, PACKET **p){
  LINKED_LIST *l = NULL;
  check_null_pointer(pf);
  try (linked_list_man_get_free_entry(&pf->list, &l) );
  if (l)
    *p = container_of(l, PACKET, list_node);
  else
    *p = malloc_gc (sizeof(PACKET));
  if (! *p)
    return ERR_MALLOC_FAIL;
  packet_init(*p);
  return SUCCESS;
}

/**
 * Insert a packet into packet-list
 * @param pf : packet list
 * @param p : packet
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_insert_packet (PACKET_LIST *pf, PACKET *p) {
  check_null_pointer(pf);
  //check_null_pointer(p);
  if (!p){
    iprint(LEVEL_ERROR, "p is NULL\n");
    return ERR_POINTER_NULL;
  }
  try ( linked_list_man_insert(&pf->list, &p->list_node) );
  pf->size++;
  pf->total_burst+= p->info.burst;
  return SUCCESS;
}

int packet_list_insert_head (PACKET_LIST *pf, PACKET *p) {
  check_null_pointer(pf);
  check_null_pointer(p);
  try ( linked_list_man_insert_head(&pf->list, &p->list_node) );
  pf->size++;
  pf->total_burst+= p->info.burst;
  return SUCCESS;
}

int packet_list_move_head (PACKET_LIST *pl, PACKET *p) {
  check_null_pointer(pl);
  check_null_pointer(p);
  linked_list_man_move_head(&pl->list, &p->list_node);
  return SUCCESS;
}
/**
 * Remove one packet out of packet list.
 * @param pf : packet list
 * @param p : packet
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_remove_packet (PACKET_LIST *pf, PACKET *p) {
  check_null_pointer(pf);
  check_null_pointer(p);
  linked_list_man_remove(&pf->list, &p->list_node);
  pf->size--;
  pf->total_burst-=p->info.burst;
  return SUCCESS;
}

/**
 * Get one packet from packet list.
 * @param pf : packet list
 * @param p : packet
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_get_first (PACKET_LIST *pf, PACKET **p) {
  LINKED_LIST *llp = NULL;
  check_null_pointer(pf);
  try ( linked_list_man_get_first(&pf->list, &llp) );
  *p = container_of(llp, PACKET, list_node);
  return SUCCESS;
}

/**
 * Check packet-list empty or not.
 * @param l : packet list
 * @return negative number if there are some errors. Return 1 if list is empty, otherwise return 0.
 */
int packet_list_is_empty (PACKET_LIST *l) {
  check_null_pointer(l);
  return (linked_list_is_empty((&l->list.entries)));
}

/**
 * Configure packet list
 * @param pl : Packet list
 * @param conf : see libs/list/linked_list.h for more info about this configuration
 * @return Error code (defined in def.h and libs/error.h)
 */
int packet_list_config (PACKET_LIST *pl, int conf) {
  check_null_pointer(pl);
  pl->list.conf = conf;
  return SUCCESS;
}

/**
 * Unit test of function packet_list_new_packet
 * @return Error code (defined in def.h and libs/error.h)
 */
int test_packet_list_new_packet () {
  PACKET_LIST pl;
  PACKET *p = NULL;
  packet_list_init(&pl, -1);
  packet_list_new_packet(&pl, &p);
  if (p != NULL)
    iprint(LEVEL_WARNING, "test_packet_list_new_packet: successful \n");
  return SUCCESS;
}

/**
 * Collect data (packet) for measurement (from appropriate queue containing
 * this packet
 * @param p : Packet
 * @return Error code (see more in def.h and error.h)
 */
int measurement_self_collect_data (PACKET *p) {
  QUEUE_TYPE *qt = NULL;
  TIME curr_time;
  MEASURES *m = NULL;
  //check_null_pointer(p);
  //check_null_pointer(m);
  qt = p->info.queue;
  m = &((FIFO_QINFO *)(qt->info))->measurement;

  switch (p->info.state) {
  case PACKET_STATE_PROCESSING:
    curr_time.real = p->info.stime.real;
    break;
  case PACKET_STATE_WAITING:
    curr_time.real = p->info.atime.real;
    break;
  case PACKET_STATE_DROPPED:
    curr_time.real = p->info.atime.real;
    break;
  case PACKET_STATE_IN:
    curr_time.real = p->info.atime.real;
    break;
  case PACKET_STATE_OUT:
    curr_time.real = p->info.etime.real;
    break;
  default:
    iprint(LEVEL_WARNING, "Packet state is not support\n");
    return ERR_PACKET_STATE_WRONG;
    break;
  }
  measurement_collect_data(m, p, curr_time);
  return SUCCESS;
}

int packet_list_reset_browsing(PACKET_LIST *el) {
  check_null_pointer(el);
  linked_list_reset_browsing(&el->list.entries);
  return SUCCESS;
}

PACKET* packet_list_get_next(PACKET_LIST *el) {
  PACKET *p = NULL;
  LINKED_LIST *next = NULL, *l = NULL;
  if (!el) return NULL;
  l = &el->list.entries;
  next = linked_list_get_next(l);
  if (next == NULL) return NULL;
  p = container_of(next, PACKET, list_node);
  return p;
}

int packet_list_test () {
  PACKET_LIST pl;
  PACKET p1, p2, p3, p4, p5;
  PACKET *p = NULL, *pt = NULL;
  packet_list_init(&pl, LL_CONF_STORE_ENTRY);
  packet_init(&p1);
  packet_init(&p2);
  packet_init(&p3);
  packet_init(&p4);
  packet_init(&p5);
  packet_list_insert_packet(&pl, &p1);
  packet_list_insert_packet(&pl, &p2);
  packet_list_insert_packet(&pl, &p3);
  packet_list_insert_packet(&pl, &p4);
  packet_list_insert_packet(&pl, &p5);
  p1.info.id = 1;
  p2.info.id = 2;
  p3.info.id = 3;
  p4.info.id = 4;
  p5.info.id = 5;
  packet_list_reset_browsing(&pl);
  for(;p = packet_list_get_next(&pl);){
    printf("Packet id %d\n", p->info.id);
    if (p->info.id == 3 || p->info.id == 1 || p->info.id == 4) {
      pt = p;
      packet_list_move_head(&pl, p);
    }
  }

  printf("New list\n");
  packet_list_reset_browsing(&pl);
  for(;p = packet_list_get_next(&pl);) {
    if (p->info.id == 4) pt = p;
    printf("Packet id %d/%d\n", p->info.id, pl.size);
  }

  printf("New list\n");
  //packet_list_remove_packet(&pl, pt);
  packet_list_insert_head(&pl, pt);
  packet_list_reset_browsing(&pl);
  for(;p = packet_list_get_next(&pl);) {
    if (p->info.id == 4) pt = p;
    printf("Packet id %d/%d\n", p->info.id, pl.size);
  }
  return SUCCESS;
}
