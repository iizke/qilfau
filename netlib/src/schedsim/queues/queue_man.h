/**
 * @file queue_man.h
 * Manage all types of queue, for example: FIFO, LIFO, RR,...
 * At this time, only FIFO is implemented.
 *
 * @date Created on: Oct 7, 2011
 * @author iizke
 */

#ifndef SCHED_QUEUE_MAN_H_
#define SCHED_QUEUE_MAN_H_

#include "list/linked_list.h"

/// Queue type FIFO
#define QUEUE_FIFO                      1
/// Queue type Round-Robin
#define QUEUE_ROUND_ROBIN               2

/// An alias of struct queue_type

typedef struct sched_queue_type SQUEUE_TYPE;

/**
 * Structure of a queue type. Can be seen as an interface of a queue object
 */
struct sched_queue_type {
  /// This element is used to join to a queue-manager
  LINKED_LIST list_node;
  /// Queue ID
  int id;
  /// Type of queue. Now, only support QUEUE_FIFO
  int type;
  /// Initialize queue type, include info in this structure
  int (*init) (SQUEUE_TYPE *);
  /// Check whether a queue is idle or not
  int (*is_idle) (SQUEUE_TYPE *);
  /// Push a packet into queue
  int (*push_packet) (SQUEUE_TYPE*, void *);
  /// Process packet getting from queue
  int (*process_packet) (SQUEUE_TYPE*, void *);
  /// Finish processing packet
  int (*finish_packet) (SQUEUE_TYPE*, void *);
  /// Get current queue length
  int (*get_waiting_length) (SQUEUE_TYPE*);
  /// Get a packet from waiting queue and remove it out of list
  int (*select_waiting_packet) (SQUEUE_TYPE*, void **);
  /// Get a packet from executing queue
  int (*get_executing_packet) (SQUEUE_TYPE*, void**);
  /// Get a packet from waiting queue, but packet still in queue
  int (*get_waiting_packet) (SQUEUE_TYPE*, void **);
  //JOB* (*find_executing_packet_to) (SQUEUE_TYPE*, int);
  //void *info;
};

/**
 * Queue manager structure
 */
typedef struct sched_queue_type_list {
  /// Manager of list of queue-type
  LINKED_LIST_MAN list;
  /// Current active queue_type
  SQUEUE_TYPE *curr_queue;
} SQUEUE_MAN;

int squeue_man_init (SQUEUE_MAN *qm);
int squeue_man_register_new_type (SQUEUE_MAN *qm, SQUEUE_TYPE *qi);
int squeue_man_unregister_type (SQUEUE_MAN *qm, SQUEUE_TYPE *qi);
int squeue_man_activate_type (SQUEUE_MAN *qm, int type);

#endif /* QUEUE_MAN_H_ */
