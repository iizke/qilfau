/**
 * @file netqthread.c
 * Discrete event scheduling simulation of network queue (parallel implementation)
 *
 * Algorithm:
 *
 * Data structure for each node:
 * - Local clock
 * - Queue (service time, capacity)
 * - Event list: arrival and end-service => need to be synchronized to avoid
 * causal violation.
 *
 * At each node (transit), do arrival process, end-service process and packet process
 *
 *    Arrival process:
 *    - Push packet to queue
 *    - If queue can process a packet then do it
 *
 *    End-service process
 *    - Put packet out of queue
 *    - If packet need to be forwarded then forward it
 *      - "Send" packet over the channel to other queue (* synch)
 *    - If queue can process a packet then do it
 *
 *    Packet process:
 *    - Generate end-service event
 *
 * There are two kinds of neighbor of node A:
 *  - From-neighbor: representing the channel from these neighbors to node A.
 *  - To-neighbor: representing the channel from node A to these neighbors.
 *
 * Procedure of scheduling events (PISAS-SCHED) at each node:
 * (1) Check exiting condition. If need to exit -> Get out of procedure.
 * (2) Get the next event. Need to be synchronized with other from-neighbors.
 *      - To-neighbor is not a problem of synchronization.
 *      - Find the safe interval:
 *          - E is "current" event in Event-list.
 *          - T(E) is time of event to be happened.
 *          - T is "current" time. To be sure that: T < T(E).
 *          - MinT is minimum time of next arrival to this node in every From-Neighbors.
 *            - For each node in From-Neighbors:
 *              - Get MinT = min (MinT, T_end_executing(to A) + min_delay) (*)
 *          - If MinT < T(E): must wait -> Exit and Move to another node (which node?
 *          should be the smallest-clock node or not?).
 *          - Else: accept this current event to be processed.
 * (3) Process this event (more details showed above).
 * (4) Remove event and Return to (1)
 *
 * Thread implementation:
 *  Shared data structures:
 *    - All queues
 *  Run:
 *    - Traverse all nodes:
 *      - If there is a thread currently running on this node (node state is RUNNING)
 *        then change to other node.
 *      - Else: change its state to RUNNING and run pisas-sched.
 *
 * @date Created on: Jun 15, 2011
 * @author iizke
 */

#include <pthread.h>
#include <time.h>
#include "error.h"
#include "netqueue.h"
#include "queues/fifo.h"
#include "event.h"

// _nq: type is NETQ_ONE_STATE
// _qid: type is int
// _from: type is int
#define nqthr_traverse_end_neighbor(_nq,_qid,_from) \
  (graph_get_end_neighbor(_nq->queuenet, _qid, _from))

#define nqthr_traverse_head_neighbor(_nq,_qid,_from) \
  (graph_get_head_neighbor(_nq->queuenet, _qid, _from))

#define nqthr_get_node (_nq,_qid) \
  (_nq->queuenet.nodes.get_node(&_nq->queuenet->nodes, _qid))

#define nqthr_node_need_wait(_nq,_neighbor) \
  ((_nq->qstate.curr_time.real - 30*_nq->qstate.measurement.servtime.avg) > _neighbor->qstate.curr_time.real)
  //((_nq->qstate.curr_time.real - 10*_nq->qstate.measurement.servtime.avg) > _neighbor->qstate.curr_time.real)

// different in seconds
#define time_diff(_start,_end) \
  (_end->tv_sec - _start->tv_sec + (_end->tv_usec-_start->tv_usec)/1000000)

/// Free packet list (used to avoiding malloc operations
PACKET_LIST free_packets;
sem_t nq_mutex;


static int nqthr_node_set_state (NETQ_ONE_STATE *node, int state) {
  //check_null_pointer(node);
  //sem_wait(&node->mutex);
  node->state = state;
  //sem_post(&node->mutex);
  return SUCCESS;
}

/**
 * Create new packet
 * @param state : System state
 * @param p : new packet
 * @return Error code (see more in def.h and error.h)
 */
static int _new_packet (PACKET **p) {
  int err = SUCCESS;
  sem_wait(&nq_mutex);
  err = packet_list_new_packet(&free_packets, p);
  sem_post(&nq_mutex);
  return err;
}

/**
 * Free a packet. This action is controlled by program to have a good performance
 * by avoiding free/malloc operations frequently
 *
 * @param state : network of queue state
 * @param p : packet
 * @return Error code (see more in def.h and error.h)
 */
static int _free_packet (PACKET *p) {
  int err = SUCCESS;
  try (linked_list_remove(&p->list_node));
  sem_wait(&nq_mutex);
  err = packet_list_remove_packet(&free_packets, p);
  sem_post(&nq_mutex);
  return err;
}

/**
 * Update local system time
 * @param e : Event
 * @param state : System state
 * @return Error code (see more in def.h and error.h)
 */
static int _update_local_time (EVENT *e, NETQ_ONE_STATE *state) {
  if (state->qstate.curr_time.real <= e->info.time.real)
    state->qstate.curr_time.real = e->info.time.real;
  else {
    iprint(LEVEL_WARNING, "Event is late, may be FES is not sorted \n");
    return ERR_EVENT_TIME_WRONG;
  }
  return SUCCESS;
}

static EVENT* nqthr_generate_arrival_from_packet (NETQ_ONE_STATE *state, PACKET *packet) {
  EVENT *e = NULL;

  if (state->state == NETQ_ONE_STATE_OFF) {
    _free_packet(packet);
    return NULL;
  }
  event_list_new_event_mutex(&state->qstate.future_events, &e);
  e->info.type = EVENT_ARRIVAL;
  e->info.time.real = packet->info.atime.real;
  packet->info.state = PACKET_STATE_IN;
  e->info.packet = packet;
  event_list_insert_event_mutex(&state->qstate.future_events, e);
  return NULL;
}

/**
 * Generate new end-service event.
 * @param p : Processing packet (for this event)
 * @param netconf : user configuration
 * @param state : System state
 * @return New event (already added in the event list)
 */
static EVENT* nqthr_generate_end_service (PACKET *p, CONFIG *conf, NETQ_ONE_STATE *state) {
  EVENT *e = NULL;
  if (state->state == NETQ_ONE_STATE_OFF) {
    _free_packet(p);
    return NULL;
  }
  if (event_list_new_event_mutex(&state->qstate.future_events, &e) < 0) {
    _free_packet(p);
    //event_list_remove_event(&state->qstate.future_events, e);
    e = NULL;
  } else if (event_setup(e, &conf->service_conf, state->qstate.curr_time) < 0) {
    _free_packet(p);
    event_list_remove_event_mutex(&state->qstate.future_events, e);
    e = NULL;
  } else {
    e->info.type = EVENT_END_SERVICE;
    e->info.packet = p;
    p->info.etime.real = e->info.time.real;
    event_list_insert_event_mutex(&state->qstate.future_events, e);
  }
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param cnf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
static int nqthr_allow_continue (CONFIG *cnf, SYS_STATE_OPS *ops) {
  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
  NETQ_ONE_STATE *neighbor = NULL;
  // Assume that configuration of exiting conditions is stored in the first nodes's configuration
  STOP_CONF *stop_conf = &cnf->stop_conf;
  int from = 0;
  int qid = state->qstate.queues.curr_queue->id;

  // Just a safe condition: it should not be processed
  if (state->state == NETQ_ONE_STATE_OFF)
    return 0;
  if (state->state == NETQ_ONE_STATE_WAITING) {
    nqthr_node_set_state(state, NETQ_ONE_STATE_ON);
    //state->state = NETQ_ONE_STATE_ON;
    return 0;
  }

  // valley algorithm
  if (state->waited_node > 0) {
    // check waited node
    neighbor = state->queuenet->nodes.get_node(&state->queuenet->nodes, state->waited_node);
    if (nqthr_node_need_wait(state, neighbor)) {
      nqthr_node_set_state(state, NETQ_ONE_STATE_ON);
      return 0;
    } else
      state->waited_node = -1;
  }

  /*
   * If it is transit node, then turn off this node when all head neighbors are off
   * and there is no packets in queue.
   */
  if (cnf->nodetype != NODE_SOURCE) {
    if (!event_list_is_empty(&state->qstate.future_events))
      return 1;
    while ((neighbor = nqthr_traverse_head_neighbor(state, qid, from))) {
      if (neighbor->state != NETQ_ONE_STATE_OFF) {
        nqthr_node_set_state(state, NETQ_ONE_STATE_ON);
        return 0;
      }
      from++;
    }
    nqthr_node_set_state(state, NETQ_ONE_STATE_OFF);
    //state->state = NETQ_ONE_STATE_OFF;
    return 0;
  }

  /*
   * Source node: no generate more packets and event
   */
  if ((stop_conf->max_time > 0 && state->qstate.curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->qstate.measurement.total_arrivals > stop_conf->max_arrival)) {
    // By doing this, source nodes will not generate more packet when the system need to finish
    cnf->service_conf.type = RANDOM_OTHER;
    nqthr_node_set_state(state, NETQ_ONE_STATE_OFF);
    printf("Source Queue off \n");
    //state->state = NETQ_ONE_STATE_OFF;
    return 0;
  }
  return 1;
}

static int nqthr_routing_to_node (NETQ_ONE_STATE *state) {
  // TODO: Routing here. But now we consider only one link/channel from node-node
  int qid = state->qstate.queues.curr_queue->id;
  NETQ_ONE_STATE *next = nqthr_traverse_end_neighbor(state, qid, 0);
  if (!next) {
    // There is no end neighbor -> cannot forward packet (only apply for transit/source node
    state->state = NETQ_ONE_STATE_OFF;
    return ERR_INVALID_VAL;
  }
  return next->qstate.queues.curr_queue->id;
}

/**
 * Process a packet
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
static int nqthr_process_packet (CONFIG *cnf, NETQ_ONE_STATE *state) {
  //QUEUE_TYPE *qt = netq_get_queue(state, qid);
  EVENT *e = NULL;
  PACKET *p = NULL;
  QUEUE_TYPE *qt = state->qstate.queues.curr_queue;
  int to_node = 0;
  int count = 0;
  int max = 0;

  switch (cnf->nodetype) {
  case NODE_SOURCE:
    if ((max = cnf->queue_conf.max_waiters) < 0)
      max = 10;
    if (qt->get_waiting_length(qt) == 0) {
      while (count < max) {
        to_node = nqthr_routing_to_node(state);
        if (to_node < 0)
          return ERR_INVALID_VAL;
        try (_new_packet(&p));
        packet_init(p);
        p->info.atime.real = state->qstate.curr_time.real;
        p->info.queue = qt;
        p->info.to_queue = to_node;
        qt->push_packet(qt, p);
        if (p->info.state == PACKET_STATE_DROPPED) {
          _free_packet(p);
          break;
        }
        state->qstate.measurement.total_arrivals++;
        count++;
      }
    }
  case NODE_TRANSIT:
    qt->select_waiting_packet(qt, &p);
    p->info.stime.real = state->qstate.curr_time.real;
    qt->process_packet(qt, p);
    break;
  case NODE_SINK:
  default:
    return ERR_INVALID_VAL;
  }
  measurement_collect_data(&state->qstate.measurement, p, state->qstate.curr_time);
  e = nqthr_generate_end_service (p, cnf, state);
  return SUCCESS;
}

/**
 * Process Arrival event
 * @param e : Event
 * @param netconf : configuration from user
 * @param state : system state
 * @return Error code (see more in def.h and libs/error.h)
 */
int nqthr_process_arrival (EVENT *e, CONFIG *conf, NETQ_ONE_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = state->qstate.queues.curr_queue;

  try ( _update_local_time(e, state) );
  measurement_collect_data(&state->qstate.measurement, packet, state->qstate.curr_time);

  if (conf->nodetype != NODE_SINK)
  if ((packet->info.to_queue = nqthr_routing_to_node(state)) < 0) {
    _free_packet(packet);
    return SUCCESS;
  }

  qt->push_packet(qt, packet);
  measurement_collect_data(&state->qstate.measurement, packet, state->qstate.curr_time);
  if (packet->info.state == PACKET_STATE_DROPPED)
    _free_packet(packet);

  if ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) >= 1))
    nqthr_process_packet(conf, state);

  return SUCCESS;
}

/**
 * Process end-service event
 * @param e : event
 * @param netconf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
int nqthr_process_end_service (EVENT *e, CONFIG *cnf, NETQ_ONE_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = state->qstate.queues.curr_queue;
  int qid = qt->id;
  CHANNEL_CONF *ch = NULL;
  NETQ_ONE_STATE *neighbor = NULL;

  try ( _update_local_time(e, state) );
  packet->info.etime.real = e->info.time.real;
  qt->finish_packet(qt, packet);
  measurement_collect_data(&state->qstate.measurement, packet, state->qstate.curr_time);
  //try ( _free_packet(state, packet) );

  // forwarding packet
  neighbor = state->queuenet->nodes.get_node(&state->queuenet->nodes, packet->info.to_queue);
  if ((!neighbor) || (neighbor->state == NETQ_ONE_STATE_OFF)) {
    _free_packet(packet);
  }else {
    ch = state->queuenet->edges.get_edge(&state->queuenet->edges, qid, packet->info.to_queue);
    packet->info.atime.real = e->info.time.real + ch->delay.distribution.gen(&ch->delay.distribution);
    packet->info.ctime.real = 0;
    packet->info.stime.real = 0;
    packet->info.etime.real = 0;
    packet->info.queue = neighbor->qstate.queues.curr_queue;
    packet->info.state = PACKET_STATE_IN;
    nqthr_generate_arrival_from_packet(neighbor, packet);
    // Check balance time condition
    //printf("node %d, service time %f, curr %f \n", state->qstate.queues.curr_queue->id, state->qstate.measurement.servtime.avg, state->qstate.curr_time.real);
    if (nqthr_node_need_wait(state, neighbor)) {
    //if ((state->qstate.curr_time.real) > neighbor->qstate.curr_time.real) {
      nqthr_node_set_state(state, NETQ_ONE_STATE_WAITING);
      state->waited_node = packet->info.to_queue;
      //return SUCCESS;
    }
  }

  if (cnf->nodetype == NODE_SOURCE)
    nqthr_process_packet(cnf, state);
  else
    while ((qt->is_idle(qt) && (qt->get_waiting_length(qt) > 0)))
      nqthr_process_packet(cnf, state);

  return SUCCESS;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int nqthr_system_clean (CONFIG *conf, SYS_STATE_OPS *ops) {
//  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
//
//  if (conf->arrival_conf.to_file)
//    fclose(conf->arrival_conf.to_file);
//  if (conf->queue_conf.out_file)
//      fclose(conf->queue_conf.out_file);

  return SUCCESS;
}

/**
 * Generate new event.
 * @param type : type of event
 * @param p : Packet related to this new event
 * @param conf : user configuration
 * @param ops : Abstract system operations
 * @return New event
 */
EVENT * nqthr_generate_event(int type, PACKET *p, CONFIG *cnf, SYS_STATE_OPS *ops) {
  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
  EVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    //e = _generate_arrival( conf, state);
    e = NULL;
    break;
  case EVENT_END_SERVICE:
    e = nqthr_generate_end_service(p, cnf, state);
    break;
  default:
    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", type);
    break;
  }
  return e;
}

/**
 * Process an event.
 * @param e : Event
 * @param conf : User configuration
 * @param ops :Abstract system operations
 * @return Error code (more in def.h and error.h)
 */
int nqthr_process_event (EVENT *e, CONFIG *conf, SYS_STATE_OPS *ops) {
  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    nqthr_process_arrival(e, conf, state);
    break;
  case EVENT_END_SERVICE:
    nqthr_process_end_service(e, conf, state);
    break;
  default:
    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", e->info.type);
    break;
  }
  return SUCCESS;
}

/**
 * Get next event from event list
 * @param ops : Abstract system operations
 * @param e : returned event
 * @return Error code (more in def.h and error.h)
 */
int nqthr_get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
  NETQ_ONE_STATE *neighbor = NULL;
  QUEUE_TYPE *nqt = NULL;
  PACKET *p = NULL;
  int qid = state->qstate.queues.curr_queue->id;
  EVENT *ev = NULL;
  int from = 0;
  float min = 0;

  try ( event_list_get_first_mutex(&state->qstate.future_events, &ev) );
  min = ev->info.time.real;
  while ((neighbor = (nqthr_traverse_head_neighbor(state, qid, from)))) {
    // find processing packet that come to this node (qid) from node neighbor
    nqt = neighbor->qstate.queues.curr_queue;
    if (neighbor->state != NETQ_ONE_STATE_OFF) {
      p = nqt->find_executing_packet_to(nqt, qid);
      //CHANNEL_CONF *ch = state->queuenet->edges.get_edge(&state->queuenet->edges, nqt->id, qid);
      if (p) {
        // TODO: 0 for delay
        float next_arrival = p->info.etime.real + 0;
        if (min > next_arrival)
          min = next_arrival;
      }
    }
    from = nqt->id + 1;
  }

  if ((from == 0) && (neighbor == NULL)) {
    // detect source node: only allow to generate 1 packet per time
    nqt = state->qstate.queues.curr_queue;
    if (nqt->get_waiting_length(nqt) == 1)
      nqthr_node_set_state(state, NETQ_ONE_STATE_WAITING);
  }

  if (min >= ev->info.time.real)
    *e = ev;
  else {
    *e = NULL;
    nqthr_node_set_state(state, NETQ_ONE_STATE_WAITING);
    //state->state = NETQ_ONE_STATE_WAITING;
  }

  return SUCCESS;
}

/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
int nqthr_remove_event (SYS_STATE_OPS *ops, EVENT *e) {
  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);
  try (event_list_remove_event_mutex(&state->qstate.future_events, e) );
  return SUCCESS;
}

static int nqthr_node_init (NETQ_ONE_STATE *node, CONFIG *conf, NETQ_ALL_STATE *state) {
  check_null_pointer(node);
  sys_state_init(&node->qstate, conf);
  node->state = NETQ_ONE_STATE_ON;
  sem_init(&node->mutex, 1, 1);
  node->waited_node = -1;
  node->queuenet = &state->queuenet;
  // Reconfigure the interface
  node->qstate.ops.allow_continue = nqthr_allow_continue;
  node->qstate.ops.clean = nqthr_system_clean;
  node->qstate.ops.generate_event = nqthr_generate_event;
  node->qstate.ops.get_next_event = nqthr_get_next_event;
  node->qstate.ops.process_event = nqthr_process_event;
  node->qstate.ops.remove_event = nqthr_remove_event;
  return SUCCESS;
}

static int nqthr_build_topology (NETQ_ALL_STATE *state, NET_CONFIG *netconf) {
  // Assume that state is ready (finishing allocation memory), just fill the data
  LINKED_LIST *link = NULL;
  NETQ_ONE_STATE *node = NULL;
  CONFIG *cnf = NULL;
  int i = 0;
  // configure nodes
  for (i=0; i < netconf->nnodes; i++) {
    node = array_get(&state->nodes, i);
    cnf = netconfig_get_conf(netconf, i);
    // avoid to set up wrong parameter for sink-node
    if (cnf->nodetype == NODE_SINK) {
      cnf->queue_conf.num_servers = 0;
      cnf->queue_conf.max_waiters = 0;
    }
    nqthr_node_init(node, cnf, state);
    state->queuenet.nodes.set_node(&state->queuenet.nodes, i, node);
    node->qstate.queues.curr_queue->id = i;
  }
  // configure channel
  link = netconf->channels.next;
  while (&netconf->channels != link) {
    CHANNEL_CONF *c = container_of(link, CHANNEL_CONF, list_node);
    state->queuenet.edges.set_edge(&state->queuenet.edges, c->src, c->dest, c);
    link = link->next;
  }
  return SUCCESS;
}

static int nqthr_create_initial_events (NETQ_ALL_STATE * state, NET_CONFIG *netconf) {
  int i = 0;
  NETQ_ONE_STATE *node = NULL;
  CONFIG *cnf = NULL;
  QUEUE_TYPE *qt = NULL;
  //PACKET *packet = NULL;

  for (i=0; i< netconf->nnodes; i++) {
    node = state->queuenet.nodes.get_node(&state->queuenet.nodes, i);
    qt = node->qstate.queues.curr_queue;
    cnf = netconfig_get_conf(netconf, i);

    if (cnf->nodetype == NODE_SOURCE)
      nqthr_process_packet(cnf, node);
  }

  return SUCCESS;
}

/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int nqthr_state_init (NETQ_ALL_STATE *state, NET_CONFIG *netconf) {
  check_null_pointer(state);
  check_null_pointer(netconf);
  extern sem_t nq_mutex;

  packet_list_init(&free_packets, LL_CONF_STORE_FREE);
  sem_init(&nq_mutex, 1, 1);
  // init netq_all_state and netq_one_state
  array_setup(&state->nodes, sizeof(NETQ_ONE_STATE), netconf->nnodes);
  graph_setup_matrix(&state->queuenet, netconf->nnodes);
  /* Build topology */
  nqthr_build_topology(state, netconf);
  /* Init the trigger/initial events */
  nqthr_create_initial_events(state, netconf);

  return SUCCESS;
}

static void* nqthr_thread_run (NETQ_ALL_STATE *state) {
  /*
   * Algorithm:
   * Traverse over all nodes, if its state is RUNNING/OFF then skip it
   * if its state is ON thn simulate on it.
   */
  int i = -1, j = 0, k = 0;
  int off_nodes = 0;
  NETQ_ONE_STATE *node = NULL;
  SYS_STATE_OPS *ops = NULL;
  CONFIG *cnf = NULL;
  int *collisions;
  struct timeval start, end;
  double simulation_time;
  extern NET_CONFIG netconf;
  collisions = malloc_gc(sizeof(int)*netconf.nnodes);

  while (off_nodes < netconf.nnodes) {
    i = (i+1) % netconf.nnodes;
    k = (i + netconf.nnodes - 1) % netconf.nnodes;
    j = (i+1) % netconf.nnodes;
    if (collisions[i] > collisions[j])
      i = j;
    if (collisions[i] > collisions[k])
      i = k;
    node = state->queuenet.nodes.get_node(&state->queuenet.nodes, i);
    ops = &node->qstate.ops;
    cnf = netconfig_get_conf(&netconf, i);
    if (sem_trywait(&node->mutex) < 0) {
      collisions[i]++;
      continue;
    }

    switch (node->state){
    case NETQ_ONE_STATE_ON:
      off_nodes = 0;
      node->state = NETQ_ONE_STATE_RUNNING;
      sem_post(&node->mutex);
      break;
    case NETQ_ONE_STATE_OFF:
      off_nodes++;
      sem_post(&node->mutex);
      continue;
    case NETQ_ONE_STATE_RUNNING:
    case NETQ_ONE_STATE_WAITING:
      off_nodes = 0;
    default:
      sem_post(&node->mutex);
      continue;
    }
    // now we sure that only one thread can do the following simulation:
    //gettimeofday(&start, NULL);
    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    pisas_sched(cnf, ops);
    //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    //gettimeofday(&start, NULL);
    //simulation_time += time_diff((&start),(&end));
  }
  //printf("Number of collision: ");
  //for (i=0; i<netconf.nnodes; i++)
  //  printf("%d, ", collisions[i]);
  //printf("\n Simulation time %f \n", simulation_time);
  return NULL;
}

int nqthr_start(char *f) {
  extern NET_CONFIG netconf;
  NETQ_ALL_STATE state;
  //SYS_STATE_OPS *ops = NULL;
  pthread_t *threads;
  CONFIG *cnf = NULL;
  int i = 0;
  time_t start;
  start = time(NULL);

  netconfig_init(&netconf, 5);
  netconfig_parse_nodes("src/netsim/conf/source.conf");
  netconfig_parse_nodes("src/netsim/conf/source1.conf");
  netconfig_parse_nodes("src/netsim/conf/node1.conf");
  netconfig_parse_nodes("src/netsim/conf/node2.conf");
  netconfig_parse_nodes("src/netsim/conf/sink.conf");
  netconfig_parse_channels("src/netsim/conf/netconf.conf");
  nqthr_state_init(&state, &netconf);

  cnf = netconfig_get_conf(&netconf, 0);
  threads = malloc_gc(sizeof(pthread_t)*cnf->nthreads);
  check_null_pointer(threads);

  for (i = 0; i < cnf->nthreads; i++)
      pthread_create(&threads[i], NULL, nqthr_thread_run, &state);

  for (i = 0; i < cnf->nthreads; i++) {
    pthread_join(threads[i], NULL);
    printf("Thread %d finished.\n", i);
  }

//  nqthr_thread_run(&state);

  printf("ALL SYSTEM\n");
  for (i=0; i<netconf.nnodes; i++) {
    NETQ_ONE_STATE *node = state.queuenet.nodes.get_node(&state.queuenet.nodes, i);
    QUEUE_TYPE *qt = node->qstate.queues.curr_queue;
    printf("Queue %d \n", qt->id);
    print_measurement(&((FIFO_QINFO*)qt->info)->measurement);
    print_statistical_value("# events", &node->qstate.future_events.snum_events, 0.9);
  }
  netsim_print_theorical_mm1 (80, 100);
  printf("Time of simulation: %d (seconds) \n", time(NULL)-start);
  return SUCCESS;
}
