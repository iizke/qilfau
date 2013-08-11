/*
 * des.c
 *  Discrete Event Simulation framework
 *  Created on: Aug 7, 2013
 *      Author: iizke
 */

#include "error.h"
#include "des.h"

int des_setup(DES* des) {
  if (!des) return -1;
  des->context = NULL;
  des->curtime = 0;
  event_list2_setup(&des->eventq);
  event_ctrl_list_setup(&des->control_list);
  return SUCCESS;
}

int event_list2_setup(EVENT_LIST2 *el) {
  check_null_pointer(el);
  try ( linked_list_man_init(&el->list) );
  sem_init(&el->mutex, 0, 1);
  stat_num_init(&el->snum_events);
  el->num_events = 0;
  return SUCCESS;
}

int event_ctrl_list_setup(EVENT_CTRL_LIST* ecl) {
  check_null_pointer(ecl);
  ecl->max_active_slot = 2;
  ecl->max_slot = 4;
  return SUCCESS;
}

static SYS_ID event_ctrl_list_insert(EVENT_CTRL_LIST *ecl, int type, int (*process)(EVENT2*, void*)) {
  int id;
  int i = 0;
  // check existing id
  for (i = 0; i < ecl->max_active_slot; i++) {
    if (ecl->list[i].type == type)
      iprint(LEVEL_WARNING, "This event type (%d) is existing. Still use it, no change\n", type);
      return i;
  }
  id = ecl->max_active_slot;
  ecl->max_active_slot++;
  if (ecl->max_active_slot <= ecl->max_slot) {
    ecl->list[id].type = type;
    ecl->list[id].process = process;
  } else {
    // have to malloc new memory slot
  }
  return id;
}

SYS_ID des_reg_event (DES* des, int type, int (*process)(EVENT2*, void*)) {
  check_null_pointer(des);
  check_null_pointer(process);
  int id = event_ctrl_list_insert(&des->control_list, type, process);
  return id;
}

int des_reg_context (DES* des, void* context){
  check_null_pointer(des);
  des->context = context;
  return SUCCESS;
}


int des_recv_new_event (DES* des, EVENT2* e) {
  check_null_pointer(des);
  check_null_pointer(e);
  linked_list_man_insert(&des->eventq.list, &e->list_node);
  return SUCCESS;
}

/**
 * Get the first event in event-queue to process
 */
static int des_get_event (DES *des, EVENT2 **e) {
  check_null_pointer(des);
  linked_list_man_get_first(&des->eventq.list, e);
  return SUCCESS;
}

static int des_process_event (DES* des, EVENT2 *e) {
  if (des->curtime < e->time) des->curtime = e->time;
  // search event-type in control-list

  // do the process of event

  return SUCCESS;
}

int des_run (DES *des) {
  if (!des) return -1;
  while (des->eventq.num_events > 0) {
    EVENT2 *e = NULL;
    if (des_get_event (des, &e) < 0) break;
    if (e->type == E_END) break;
    des_process_event(des, &e);
  }
  return SUCCESS;
}
