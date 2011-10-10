/**
 * @file fifo.h
 * FIFO queue structure
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#ifndef SCHED_FIFO_H_
#define SCHED_FIFO_H_

#include "queue_man.h"
#include "job.h"
#include "measures.h"

#define QUEUE_DEFAUT_STRUCT \
                      SQUEUE_TYPE queue_type; \
                      /** Incomming packet list */ \
                      JOB_LIST incomming_packets; \
                      /** Waiting packet list */ \
                      JOB_LIST waiting_packets; \
                      /** Executing packet list */ \
                      JOB_LIST executing_packets; \
                      /** Dropped packet list */ \
                      JOB_LIST dropped_packets; \
                      /** Departure packet list */ \
                      JOB_LIST departure_packets; \
                      /** Queue measurement */ \
                      SCHED_MEASURES measurement; \
                      /** Queue state */ \
                      int state; \
                      /** Maximum number of executing packets */\
                      int max_executing; \
                      /** Maximum number of waiting packets */ \
                      int max_waiting;

/**
 * FIFO queue structure
 */
typedef struct fifo_queue {
  QUEUE_DEFAUT_STRUCT
} QUEUE_FF;

#define queue_type_get_fifo_queue(qt) (container_of(qt, QUEUE_FF, queue_type))

int sched_fifo_init (SQUEUE_TYPE **q, int max_executing, int max_waiting);
int sched_fifo_setup (SQUEUE_TYPE *q, int max_executing, int max_waiting);

#endif /* FIFO_H_ */
