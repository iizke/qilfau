/**
 * @file config.h
 * Define user configurations
 *
 * @date Created on: Apr 16, 2011
 * @author iizke
 */

#ifndef MYCONFIG_H_
#define MYCONFIG_H_

#include <stdio.h>
#include "random.h"
#include "list/array.h"
#include "list/linked_list.h"

/*
 * Definition of some constants used in type of RANDOM_CONF
 */
#define RANDOM_OTHER              1
#define RANDOM_MARKOVIAN          2
#define RANDOM_UNIFORM            3
#define RANDOM_FILE               4
#define RANDOM_BERNOULLI          5
#define RANDOM_CONST              6
#define RANDOM_MMPP               7
#define RANDOM_MMPP_R             8
#define RANDOM_POISSON            9

#define PROTOCOL_CSMA             0
#define PROTOCOL_ONE_QUEUE        1
#define PROTOCOL_NET_QUEUE        2
#define PROTOCOL_BABSQ            3

#define STOP_QUEUE_ZERO           0
#define STOP_QUEUE_NONZERO        1

#define NODE_SOURCE               1
#define NODE_TRANSIT              2
#define NODE_SINK                 3

/**
 * Flow configuration is used to characterize a flow: what is its
 * distribution, define some parameters.
 */
typedef struct random_config {
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
} RANDOM_CONF;

/**
 * Queue configuration
 */
typedef struct queue_config {
  /// type of queue, now only support QUEUE_FIFO
  int type;
  /// number of servers in a system
  int num_servers;
  /// maximum number of clients allowing to wait in a system
  int max_waiters;
  /// file name used to store event of departure flow
  FILE *out_file;
} QUEUE_CONF;

/**
 * Define conditions of stopping program
 */
typedef struct stop_config {
  /// Maximum time is allowed to run simulation
  float max_time;
  /// Maximum number of arrival events
  int max_arrival;
  /// Stop when queue length is zero
  int queue_zero;
} STOP_CONF;

typedef struct csma_conf {
  /// slot time
  double slot_time;
  /// Collision time
  double collision_time;
  /// number of queues or station
  int nstations;
  /// Backoff
  RANDOM_CONF backoff_conf;
  /// persistent probability
  RANDOM_CONF persistent_conf;
  /// ratio: propagation / avg transmission time
  /// double propagation;
} CSMA_CONF;

/**
 * Configuration from user
 */
typedef struct config {
  /// Configuration of arrival flow
  RANDOM_CONF arrival_conf;
  /// flow configuration of service time
  RANDOM_CONF service_conf;
  /// Configuration of queue system
  QUEUE_CONF queue_conf;
  /// Routing
  RANDOM_CONF routing_conf;
  /// Configuration of terminated conditions
  STOP_CONF stop_conf;
  /// Configuration of random library (IRAND, RANDLIB)
  int random_lib;
  /// CSMA configuration
  CSMA_CONF csma_conf;
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
} CONFIG;

typedef struct net_config {
  ARRAY configs;
  //int next_scan;
  /// Current number of nodes (active config)
  int nnodes;
  LINKED_LIST channels;
} NET_CONFIG;

typedef struct channel_config {
  LINKED_LIST list_node;
  int src;
  int dest;
  RANDOM_CONF delay;
  int state;
} CHANNEL_CONF;

int config_random_conf (RANDOM_CONF *rc);
int config_init (CONFIG *conf);
int config_setup (CONFIG *conf);
int config_parse_file (char *file);

CONFIG* netconfig_get_conf(NET_CONFIG *net, int cid);
int netconfig_init (NET_CONFIG *netconf, int n);
int netconfig_get_size(NET_CONFIG *net);
int netconfig_traverse_arrival(NET_CONFIG *conf, int *qid);
int netconfig_parse_nodes(char * f);
int netconfig_parse_channels(char * f);

int config_parse_babs_file (char *file);

#endif /* MYCONFIG_H_ */
