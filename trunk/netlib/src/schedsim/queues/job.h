/**
 * @file packet.h
 * Packet structure
 *
 * @date Created on: Apr 8, 2011
 * @author iizke
 */

#ifndef JOB_H_
#define JOB_H_

#include "def.h"
#include "list/linked_list.h"
#include "queue_man.h"

/// An alias of struct queue_type

//typedef struct sched_queue_type SQUEUE_TYPE;

/**
 * Packet structure
 */
typedef struct job {
  /// The element is used to connect to a packet-list
  LINKED_LIST list_node;
  /// Packet ID (currently no used)
  long id;
  /// Queue/Node that packet is stored
  SQUEUE_TYPE *queue;
  /// Destination queue id
  //int to_queue;
  /// State of packet: IN, WAITING, DROPPED, PROCESSING, OUT
  int state;
  // Service time of packet
  //float service_time;
  /// Arrival time
  double atime;
  /// Duration
  double remaining_time;
  /// Collision time
  double ctime;
  /// Start time (time when packet is processed)
  double stime;
  /// End time (end of execution time)
  double etime;
} JOB;

/**
 * Packet list structure
 */
typedef struct job_list {
  /// Manager of packet list
  LINKED_LIST_MAN list;
  /// Size of list
  int size;
  /// Port type (no used now)
  //int port_type;
  /// Port (no used now)
  //int port;
} JOB_LIST;

/*
 * State of packet in system
 */

/// Packet state IN: comming to queue
#define JOB_STATE_IN             1
/// Packet goes out of queue
#define JOB_STATE_OUT            2
/// Packet is going to be processed
#define JOB_STATE_PROCESSING     3
/// Packet in waiting list
#define JOB_STATE_WAITING        4
/// Packet is dropped
#define JOB_STATE_DROPPED        5

int job_init (JOB *p);

int job_list_init (JOB_LIST *el, int conf);
int job_list_new_job (JOB_LIST *el, JOB **e);
int job_list_insert_job (JOB_LIST *el, JOB *e);
int job_list_remove_job (JOB_LIST *el, JOB *e);
int job_list_get_first (JOB_LIST *el, JOB **e);
int job_list_is_empty (JOB_LIST *l);
int job_list_config (JOB_LIST *el, int conf);

int test_job_list_new_job ();
int smeasurement_self_collect_data (JOB *p) ;
#endif /* PACKET_H_ */
