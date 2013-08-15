/**
 * @file queue_man.h
 * Manage all types of queue, for example: FIFO, LIFO, RR,...
 * At this time, only FIFO is implemented.
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#ifndef QUEUE_MAN_H_
#define QUEUE_MAN_H_

#include "packet.h"

/// Queue type FIFO
#define QUEUE_FIFO            1
#define QUEUE_BURST_FIFO      2
/// An alias of struct queue_type
//typedef struct queue_type QUEUE_TYPE;

/**
 * Structure of a queue type. Can be seen as an interface of a queue object
 */
struct queue_type {
  /// This element is used to join to a queue-manager
  LINKED_LIST list_node;
  /// Queue ID
  int id;
  /// Type of queue. Now, only support QUEUE_FIFO
  int type;
  /// Initialize queue type, include info in this structure
  int (*init) (QUEUE_TYPE *);
  /// Check whether a queue could do serve new requests or not
    int (*is_servable) (QUEUE_TYPE *);
  /// Check whether a queue is doing sth or not
  int (*is_processing) (QUEUE_TYPE *);
  /// Push a packet into queue
  int (*push_packet) (QUEUE_TYPE*, PACKET *);
  /// Process packet getting from queue
  int (*process_packet) (QUEUE_TYPE*, PACKET *);
  /// Finish processing packet
  int (*finish_packet) (QUEUE_TYPE*, PACKET *);
  /// Get current queue length
  int (*get_waiting_length) (QUEUE_TYPE*);
  /// Get a packet from waiting queue and remove it out of list
  int (*select_waiting_packet) (QUEUE_TYPE*, PACKET **);
  /// Get a packet from executing queue
  int (*get_executing_packet) (QUEUE_TYPE*, PACKET**);
  /// Get a packet from waiting queue, but packet still in queue
  int (*get_waiting_packet) (QUEUE_TYPE*, PACKET **);
  PACKET* (*find_executing_packet_to) (QUEUE_TYPE*, int);

  void *info;
};

/**
 * Queue manager structure
 */
typedef struct queue_type_list {
  /// Manager of list of queue-type
  LINKED_LIST_MAN list;
  /// Current active queue_type
  QUEUE_TYPE *curr_queue;
} QUEUE_MAN;

int queue_man_init (QUEUE_MAN *qm);
int queue_man_register_new_type (QUEUE_MAN *qm, QUEUE_TYPE *qi);
int queue_man_unregister_type (QUEUE_MAN *qm, QUEUE_TYPE *qi);
int queue_man_activate_type (QUEUE_MAN *qm, int type);

#endif /* QUEUE_MAN_H_ */
