/**
 * @file rr.c
 *
 * @date Created on: Oct 9, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "rr.h"

/**
 * Initialization of RR queue, maybe allocate new memory if needed
 * @param q_rr : A queue type based on FIFO queue
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @param quantum: quantum
 * @return Error code (see more in def.h and error.h)
 */
int sched_rr_init (SQUEUE_TYPE **q_rr, int max_executing, int max_waiting, double quantum) {
  QUEUE_RR *qf = NULL;
  if (! *q_rr) {
    qf = malloc_gc(sizeof(QUEUE_RR));
    if (! qf)
      return ERR_MALLOC_FAIL;
    (*q_rr) = &qf->queue_type;
  }
  memset(*q_rr, 0, sizeof(SQUEUE_TYPE));
  sched_rr_setup(*q_rr, max_executing, max_waiting, quantum);
  return SUCCESS;
}

/**
 * Setup parameters of a RR queue
 * @param q_rr : RR queue type
 * @param max_executing: Maximum number of executing packets (negative value ~ infinite)
 * @param max_waiting: Maximum number of waiting packets (negative value ~ infinite)
 * @return Error code (see more in def.h and error.h)
 */
int sched_rr_setup (SQUEUE_TYPE *q_rr, int max_executing, int max_waiting, double quantum) {
  QUEUE_RR *rr_queue_info = NULL;
  check_null_pointer(q_rr);
  rr_queue_info = queue_type_get_rr_queue(q_rr);
  sched_fifo_setup(q_rr, max_executing, max_waiting);
  rr_queue_info->quantum = quantum;
  q_rr->type = QUEUE_ROUND_ROBIN;
  return SUCCESS;
}
