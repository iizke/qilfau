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

#include "error.h"
#include "netqueue.h"
#include "queues/fifo.h"

// _nq: type is NETQ_ONE_STATE
// _qid: type is int
// _from: type is int
#define nqthr_traverse_end_neighbor(_nq,_qid,_from) \
  (graph_get_end_neighbor(_nq->queuenet, _qid, _from))

#define nqthr_traverse_head_neighbor(_nq,_qid,_from) \
  (graph_get_head_neighbor(_nq->queuenet, _qid, _from))

#define nqthr_get_node (_nq,_qid) \
  (_nq->queuenet.nodes.get_node(&_nq->queuenet->nodes, _qid))

/// Free packet list (used to avoiding malloc operations
PACKET_LIST free_packets;
sem_t nq_mutex;

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

  event_list_new_event(&state->qstate.future_events, &e);
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
  if (event_list_new_event(&state->qstate.future_events, &e) < 0) {
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
    event_list_insert_event_mutex(&state->qstate.future_events, e);
  }
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
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
    state->state == NETQ_ONE_STATE_ON;
      return 0;
  }

  /*
   * If not a source node, then turn off this node when all head neighbors are off
   * and there is no packets in queue
   */
  if (cnf->nodetype != NODE_SOURCE) {
    QUEUE_TYPE *qt = state->qstate.queues.curr_queue;
    if ((qt->get_waiting_length(qt) > 0) || (!qt->is_idle(qt)))
      return 1;
    while ((neighbor = nqthr_traverse_head_neighbor(state, qid, from))) {
      if (neighbor->state != NETQ_ONE_STATE_OFF)
        return 1;
      from++;
    }
    state->state = NETQ_ONE_STATE_OFF;
    return 0;
  }

  /*
   * Source node: no generate more packets and event
   */
  if ((stop_conf->max_time > 0 && state->qstate.curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->qstate.measurement.total_arrivals > stop_conf->max_arrival)) {
    // By doing this, source nodes will not generate more packet when the system need to finish
    cnf->service_conf.type = RANDOM_OTHER;
    state->state = NETQ_ONE_STATE_OFF;
    return 0;
  }
  return 1;
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

  switch (cnf->nodetype) {
  case NODE_SOURCE:
    try (_new_packet(&p));
    packet_init(p);
    p->info.atime.real = state->qstate.curr_time.real;
    p->info.stime.real = state->qstate.curr_time.real;
    p->info.queue = qt;
    p->info.state = PACKET_STATE_PROCESSING;
    break;
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
  if (!e) {
    iprint(LEVEL_WARNING, "Cannot generate end service event \n");
    return ERR_POINTER_NULL;
  }
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
  int from = 0;
  NETQ_ONE_STATE *neighbor =  NULL;

  try ( _update_local_time(e, state) );
  packet->info.etime.real = e->info.time.real;
  if (cnf->nodetype != NODE_SOURCE)
    qt->finish_packet(qt, packet);
  measurement_collect_data(&state->qstate.measurement, packet, state->qstate.curr_time);
  //try ( _free_packet(state, packet) );

  while ((neighbor = nqthr_traverse_end_neighbor(state, qid, from))) {
    CHANNEL_CONF *ch = state->queuenet->edges.get_edge(&state->queuenet->edges,
        qid, neighbor->qstate.queues.curr_queue->id);
    packet->info.atime.real = e->info.time.real + ch->delay.distribution.gen(&ch->delay.distribution);
    packet->info.ctime.real = 0;
    packet->info.stime.real = 0;
    packet->info.etime.real = 0;
    packet->info.queue = neighbor->qstate.queues.curr_queue;
    packet->info.state = PACKET_STATE_IN;
    nqthr_generate_arrival_from_packet(neighbor, packet);
    from = neighbor->qstate.queues.curr_queue->id + 1;
    break;
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
static int nq_system_clean (CONFIG *conf, SYS_STATE_OPS *ops) {
//  NETQ_ONE_STATE *state = get_netq_one_state_from_ops(ops);

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  if (conf->queue_conf.out_file)
      fclose(conf->queue_conf.out_file);

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
  int qid = state->qstate.queues.curr_queue->id;
  EVENT *ev = NULL;
  int from = 0;
  float min = 0;

  event_list_get_first(&state->qstate.future_events, &ev);
  min = ev->info.time.real;
  while ((neighbor = (nqthr_traverse_head_neighbor(state, qid, from)))) {
    // find processing packet that come to this node (qid) from node neighbor
    QUEUE_TYPE *nqt = neighbor->qstate.queues.curr_queue;
    PACKET *p = nqt->find_executing_packet_to(nqt, qid);
    CHANNEL_CONF *ch = state->queuenet->edges.get_edge(&state->queuenet->edges, nqt->id, qid);
    if (p) {
      float next_arrival = p->info.etime.real + 0; // TODO: 0 for delay

      if (min > next_arrival)
        min = next_arrival;
    }
    from = nqt->id + 1;
  }
  if (min >= ev->info.time.real)
    *e = ev;
  else {
    *e = NULL;
    state->state = NETQ_ONE_STATE_WAITING;
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

///**
// * Initialize system state of one-queue system
// * @param state : system state
// * @param conf : user configuration
// * @return Error code (more in def.h and error.h)
// */
//int netq_state_init (NETQ_STATE *state, NET_CONFIG *netconf) {
//  int i = 0;
//  CONFIG *cnf = NULL;
//  QUEUE_TYPE *qt = NULL;
//  LINKED_LIST *link = NULL;
//  PACKET *packet = NULL;
////  QUEUE_TYPE *fifo_queue = NULL;
//  check_null_pointer(state);
//  check_null_pointer(netconf);
//
//  state->curr_time.real = 0;
//  packet_list_init(&state->free_packets, LL_CONF_STORE_FREE);
//  event_list_init(&state->future_events);
//  measures_init (&state->measurement);
//
//  array_setup(&state->queues, sizeof(QUEUE_TYPE), netconf->nnodes);
//  graph_setup_matrix(&state->queuenet, netconf->nnodes);
//  /* TODO: Build topology */
//  link = netconf->channels.next;
//  while (&netconf->channels != link) {
//    CHANNEL_CONF *c = container_of(link, CHANNEL_CONF, list_node);
//    state->queuenet.edges.set_edge(&state->queuenet.edges, c->src, c->dest, c);
//    link = link->next;
//  }
//
//  for (i=0; i< netconf->nnodes; i++) {
//    qt = array_get(&state->queues, i);
//    qt->id = i;
//    cnf = netconfig_get_conf(netconf, i);
//    state->queuenet.nodes.set_node(&state->queuenet.nodes, i, qt);
//
//    /* TODO: Setup initial events */
//    if (cnf->nodetype == NODE_SOURCE) {
//      try (_new_packet(state, &packet));
//      packet_init(packet);
//      packet->info.atime.real = state->curr_time.real;
//      packet->info.stime.real = state->curr_time.real;
//      packet->info.queue = qt;
//      packet->info.state = PACKET_STATE_PROCESSING;
//      nq_generate_end_service(packet, netconf, state);
//    } else if (cnf->nodetype == NODE_SINK) {
//      cnf->queue_conf.num_servers = 0;
//      cnf->queue_conf.max_waiters = 0;
//    }
//    fifo_setup(qt, cnf->queue_conf.num_servers, cnf->queue_conf.max_waiters);
//  }
//
//  state->ops.allow_continue = nq_allow_continue;
//  state->ops.clean = nq_system_clean;
//  state->ops.generate_event = nq_generate_event;
//  state->ops.get_next_event = nq_get_next_event;
//  state->ops.process_event = nq_process_event;
//  state->ops.remove_event = nq_remove_event;
//  return SUCCESS;
//}
//
//int netq_run(char *f) {
//  extern NET_CONFIG netconf;
//  int i = 0;
//  NETQ_STATE state;
//  SYS_STATE_OPS *ops = NULL;
//
//  netconfig_init(&netconf, 4);
//  netconfig_parse_nodes("src/netsim/conf/source.conf");
//  netconfig_parse_nodes("src/netsim/conf/node1.conf");
//  netconfig_parse_nodes("src/netsim/conf/node2.conf");
//  netconfig_parse_nodes("src/netsim/conf/sink.conf");
//  netconfig_parse_channels("src/netsim/conf/netconf.conf");
//  netq_state_init(&state, &netconf);
//  ops = &state.ops;
//  pisas_sched(&netconf, ops);
//  printf("ALL SYSTEM\n");
//  print_measurement(&state.measurement);
//  for (i=0; i<netconf.nnodes; i++) {
//    QUEUE_TYPE *qt = netq_get_queue(&state, i);
//    printf("Queue %d \n", i);
//    print_measurement(&((FIFO_QINFO*)qt->info)->measurement);
//  }
//  netsim_print_theorical_mm1 (80, 100);
//  return SUCCESS;
//}
