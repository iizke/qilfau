/**
 * @file rr.h
 * Round Robin queue structure
 *
 * @date Created on: Oct 9, 2011
 * @author iizke
 */

#ifndef RR_H_
#define RR_H_

#include "queue_man.h"
#include "job.h"
#include "measures.h"

#include "fifo.h"

/**
 * Round Robin queue structure
 */

typedef struct rr_queue {
  QUEUE_DEFAUT_STRUCT
  double quantum;
} QUEUE_RR;

#define queue_type_get_rr_queue(qt) (container_of(qt, QUEUE_RR, queue_type))

int sched_rr_init (SQUEUE_TYPE **q, int max_executing, int max_waiting, double quantum);
int sched_rr_setup (SQUEUE_TYPE *q, int max_executing, int max_waiting, double quantum);

#endif /* RR_H_ */
