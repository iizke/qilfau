/**
 * @file packet.h
 * Packet structure
 *
 * @date Created on: Apr 8, 2011
 * @author iizke
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "def.h"
#include "list/linked_list.h"
//#include "queue_man.h"

/// An alias of struct queue_type
typedef struct queue_type QUEUE_TYPE;

/**
 * Packet information
 */
typedef struct packet_info {
  /// Packet ID (currently no used)
  long id;
  /// Queue/Node that packet is stored
  QUEUE_TYPE *queue;
  /// Destination queue id
  int to_queue;
  /// State of packet: IN, WAITING, DROPPED, PROCESSING, OUT
  int state;
  // Service time of packet
  //float service_time;
  /// Arrival time
  double atime;
  /// Collision time
  double ctime;
  /// Start time (time when packet is processed)
  double stime;
  /// End time (end of execution time)
  double etime;
  /// Duration
  double remaining_time;
} PACKET_INFO;

/**
 * Packet structure
 */
typedef struct packet {
  /// The element is used to connect to a packet-list
  LINKED_LIST list_node;
  /// Packet information
  PACKET_INFO info;
} PACKET;

/**
 * Packet list structure
 */
typedef struct packet_list {
  /// Manager of packet list
  LINKED_LIST_MAN list;
  /// Size of list
  int size;
  /// Port type (no used now)
  int port_type;
  /// Port (no used now)
  int port;
} PACKET_LIST;

/*
 * State of packet in system
 */

/// Packet state IN: comming to queue
#define PACKET_STATE_IN             1
/// Packet goes out of queue
#define PACKET_STATE_OUT            2
/// Packet is going to be processed
#define PACKET_STATE_PROCESSING     3
/// Packet in waiting list
#define PACKET_STATE_WAITING        4
/// Packet is dropped
#define PACKET_STATE_DROPPED        5

int job_init (PACKET *p);

int job_list_init (PACKET_LIST *el, int conf);
int job_list_new_job (PACKET_LIST *el, PACKET **e);
int job_list_insert_job (PACKET_LIST *el, PACKET *e);
int job_list_remove_job (PACKET_LIST *el, PACKET *e);
int job_list_get_first (PACKET_LIST *el, PACKET **e);
int job_list_is_empty (PACKET_LIST *l);
int job_list_config (PACKET_LIST *el, int conf);

int test_job_list_new_job ();
int smeasurement_self_collect_data (PACKET *p) ;
#endif /* PACKET_H_ */
