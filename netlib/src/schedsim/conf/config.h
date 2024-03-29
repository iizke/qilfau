/**
 * @file config.h
 * Define user configurations
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#ifndef SCHED_MYCONFIG_H_
#define SCHED_MYCONFIG_H_

#include <stdio.h>
#include "random.h"
#include "list/array.h"
#include "list/linked_list.h"

/*
 * Definition of some constants used in type of RANDOM_SCONF
 */
#define RANDOM_OTHER              1
#define RANDOM_MARKOVIAN          2
#define RANDOM_UNIFORM            3
#define RANDOM_FILE               4
#define RANDOM_BERNOULLI          5
#define RANDOM_CONST              6
#define RANDOM_MMPP               7
#define RANDOM_MMPP_R             8

#define PROTOCOL_CSMA             0
#define PROTOCOL_ONE_QUEUE        1
#define PROTOCOL_NET_QUEUE        2

#define STOP_QUEUE_ZERO           0
#define STOP_QUEUE_NONZERO        1

//#define NODE_SOURCE               1
//#define NODE_TRANSIT              2
//#define NODE_SINK                 3

/**
 * Flow configuration is used to characterize a flow: what is its
 * distribution, define some parameters.
 */
typedef struct sched_random_config {
  /// types of flow: RANDOM_MARKOVIAN, RANDOM_UNIFORM, RANDOM_FILE, RANDOM_OTHER.
  int type;
  /// used when type is RANDOM_MARKOVIAN
  double lambda;
  /// used for RANDOM_UNIFORM
  double from;
  /// used for RANDOM_UNIFORM
  double to;
  /// used for FLOW_BERNOULLI
  double prob;
  /// Constant value
  double constval;
  /// file name that storing the this flow when it is generated
  FILE *to_file;
  /// used when type is RANDOM_FILE
  FILE *from_file;
  /// Random Distribution of flow
  RANDOM_DIST distribution;
} RANDOM_SCONF;

/**
 * Queue configuration
 */
typedef struct sched_queue_config {
  /// type of queue, now only support QUEUE_FIFO
  int type;
  /// number of servers in a system
  int num_servers;
  /// maximum number of clients allowing to wait in a system
  int max_waiters;
  /// file name used to store event of departure flow
  FILE *out_file;
  double quantum;
  void* details;
} QUEUE_SCONF;

/**
 * Define conditions of stopping program
 */
typedef struct sched_stop_config {
  /// Maximum time is allowed to run simulation
  float max_time;
  /// Maximum number of arrival events
  int max_arrival;
  /// Stop when queue length is zero
  int queue_zero;
} STOP_SCONF;

//typedef struct csma_conf {
//  /// slot time
//  double slot_time;
//  /// Collision time
//  double collision_time;
//  /// number of queues or station
//  int nstations;
//  /// Backoff
//  RANDOM_SCONF backoff_conf;
//  /// persistent probability
//  RANDOM_SCONF persistent_conf;
//  /// ratio: propagation / avg transmission time
//  /// double propagation;
//} CSMA_CONF;

/**
 * Configuration from user
 */
typedef struct sched_config {
  /// Configuration of arrival flow
  RANDOM_SCONF arrival_conf;
  /// flow configuration of service time
  RANDOM_SCONF service_conf;
  /// Configuration of queue system
  QUEUE_SCONF queue_conf;
  /// Routing
  RANDOM_SCONF routing_conf;
  /// Configuration of terminated conditions
  STOP_SCONF stop_conf;
  /// Configuration of random library (IRAND, RANDLIB)
  int random_lib;
  /// CSMA configuration
  //CSMA_CONF csma_conf;
  /// protocol: ONE_QUEUE or CSMA
  int protocol;
  /// number of threads
  int nthreads;
  /**
   * Current simulation: used for handling signal SIGINT (we dont want to wait
   * to long time, but also want to see the intermediate result
   */
  void *runtime_state;
  int nodetype;
} SCHED_CONFIG;

//typedef struct net_config {
//  ARRAY configs;
//  //int next_scan;
//  /// Current number of nodes (active config)
//  int nnodes;
//  LINKED_LIST channels;
//} NET_CONFIG;
//
//typedef struct channel_config {
//  LINKED_LIST list_node;
//  int src;
//  int dest;
//  RANDOM_SCONF delay;
//  int state;
//} CHANNEL_CONF;

int schedconf_random_conf (RANDOM_SCONF *rc);
int schedconf_init (SCHED_CONFIG *conf);
int schedconf_setup (SCHED_CONFIG *conf);
int schedconf_parse_file (char *file);

//SCHED_CONFIG* netconfig_get_conf(NET_CONFIG *net, int cid);
//int netconfig_init (NET_CONFIG *netconf, int n);
//int netconfig_get_size(NET_CONFIG *net);
//int netconfig_traverse_arrival(NET_CONFIG *conf, int *qid);
//int netconfig_parse_nodes(char * f);
//int netconfig_parse_channels(char * f);

#endif /* MYCONFIG_H_ */
