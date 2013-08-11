/*
 * onequeue.c
 *  Simulation of single queue system
 *  Created on: Aug 8, 2013
 *      Author: iizke
 */

#include "error.h"
#include "onequeue.h"

int onequeue_start (DES *des, QUEUE_STATE *qs) {
  // register events and control functions to DES
  des_reg_event(des, E_ARRIVAL, qs->arrival_process);
  des_reg_event(des, E_END_SERVICE, qs->service_process);
  des_reg_event(des, E_START, qs->start_process);
  des_reg_event(des, E_END, qs->end_process);
  des_reg_context(des, (void*)qs);
  // send start event to DES system
  EVENT2 e_start;
  e_start.type = E_START;
  e_start.time = 0;
  des_recv_new_event(des, &e_start);
  des_run(des);
  return SUCCESS;
}
