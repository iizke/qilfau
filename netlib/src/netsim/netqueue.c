/**
 * @file net.c
 * A network of queue
 * @date Created on: Jun 12, 2011
 * @author iizke
 */

#include "error.h"
#include "netqueue.h"
#include "queues/fifo.h"

//#define netq_get_neighbor_of(_nq,_id) (_nq->queuenet.edges.get_edge_from(&_nq->queuenet.edges,qid))
#define netq_traverse_neighbor_from(_nq,_qid,_from) (graph_get_end_neighbor(&_nq->queuenet, _qid, _from))
//#define netq_traverse_neighbor_to(_nq,_qid,_from) (graph_get_end_neighbor(&_nq->queuenet, _qid, _from))

/**
 * Create new packet
 * @param state : System state
 * @param p : new packet
 * @return Error code (see more in def.h and error.h)
 */
static int _new_packet (NETQ_STATE *state, PACKET **p) {
  try ( packet_list_new_packet(&state->free_packets, p) );
  return SUCCESS;
}

/**
 * Free a packet. This action is controlled by program to have a good performance
 * by avoiding free/malloc operations frequently
 *
 * @param state : network of queue state
 * @param p : packet
 * @return Error code (see more in def.h and error.h)
 */
static int _free_packet (NETQ_STATE *state, PACKET *p) {
  try (linked_list_remove(&p->list_node));
  packet_list_remove_packet(&state->free_packets, p);
  return SUCCESS;
}

/**
 * Update system time
 * @param e : Event
 * @param state : System state
 * @return Error code (see more in def.h and error.h)
 */
static int update_time (EVENT *e, NETQ_STATE *state) {
  check_null_pointer(e);
  check_null_pointer(state);
  if (state->curr_time.real <= e->info.time.real)
    state->curr_time.real = e->info.time.real;
  else {
    iprint(LEVEL_WARNING, "Event is late, may be FES is not sorted \n");
    return ERR_EVENT_TIME_WRONG;
  }
  return SUCCESS;
}

static QUEUE_TYPE * netq_get_queue (NETQ_STATE *nq, int qid) {
  if (!nq) return NULL;
  return array_get(&nq->queues, qid);
}

static EVENT* nq_generate_arrival_from_packet (NET_CONFIG *netconf, NETQ_STATE *state, PACKET *packet) {
  EVENT *e = NULL;

  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_ARRIVAL;
  e->info.time.real = packet->info.atime.real;
  packet->info.state = PACKET_STATE_IN;
  e->info.packet = packet;
  event_list_insert_event(&state->future_events, e);
  return NULL;
}

/**
 * Generate new end-service event.
 * @param p : Processing packet (for this event)
 * @param netconf : user configuration
 * @param state : System state
 * @return New event (already added in the event list)
 */
static EVENT* nq_generate_end_service (PACKET *p, NET_CONFIG *netconf, NETQ_STATE *state) {
  EVENT *e = NULL;
  CONFIG *conf = NULL;
  int qid = p->info.queue->id;
  conf = netconfig_get_conf(netconf, qid);
  if (event_list_new_event(&state->future_events, &e) < 0) {
    _free_packet(state, p);
    //event_list_remove_event(&state->future_events, e);
    e = NULL;
  } else if (event_setup(e, &conf->service_conf, state->curr_time) < 0) {
    _free_packet(state, p);
    event_list_remove_event(&state->future_events, e);
    e = NULL;
  } else {
    e->info.type = EVENT_END_SERVICE;
    e->info.packet = p;
    event_list_insert_event(&state->future_events, e);
  }
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
static int nq_allow_continue (NET_CONFIG *netconf, SYS_STATE_OPS *ops) {
  NETQ_STATE *state = get_netq_state_from_ops(ops);
  CONFIG *conf = netconfig_get_conf(netconf, 0);
  STOP_CONF *stop_conf = &conf->stop_conf;
  int i = 0;
  int n = netconf->nnodes;

  if ((stop_conf->max_time > 0 && state->curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival)) {
    // By doing this, source nodes will not generate more packet when the system need to finish
    for (i=0; i<n; i++) {
      conf = netconfig_get_conf(netconf, i);
      if (conf->nodetype == NODE_SOURCE) {
        conf->service_conf.type = RANDOM_OTHER;
      }
    }
    // This setting is to deny any events after this time
    if (!event_list_is_empty(&state->future_events)) {
      //event_list_stop_growing (&state->future_events);
      return 1;
    } else
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
static int nq_process_packet (NET_CONFIG *netconf, NETQ_STATE *state, QUEUE_TYPE *qt) {
  //QUEUE_TYPE *qt = netq_get_queue(state, qid);
  EVENT *e = NULL;
  PACKET *p = NULL;
  CONFIG *cnf = netconfig_get_conf(netconf, qt->id);

  switch (cnf->nodetype) {
  case NODE_SOURCE:
    try (_new_packet(state, &p));
    packet_init(p);
    p->info.atime.real = state->curr_time.real;
    p->info.stime.real = state->curr_time.real;
    p->info.queue = qt;
    p->info.state = PACKET_STATE_PROCESSING;
    break;
  case NODE_TRANSIT:
    qt->select_waiting_packet(qt, &p);
    p->info.stime.real = state->curr_time.real;
    qt->process_packet(qt, p);
    break;
  case NODE_SINK:
  default:
    return ERR_INVALID_VAL;
  }
  measurement_collect_data(&state->measurement, p, state->curr_time);
  e = nq_generate_end_service (p, netconf, state);
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
int nq_process_arrival (EVENT *e, NET_CONFIG *netconf, NETQ_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = packet->info.queue;

  try ( update_time(e, state) );
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->info.state == PACKET_STATE_DROPPED)
    _free_packet(state, packet);

  if ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) >= 1))
    nq_process_packet(netconf, state, qt);

  return SUCCESS;
}

/**
 * Process end-service event
 * @param e : event
 * @param netconf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
int nq_process_end_service (EVENT *e, NET_CONFIG *netconf, NETQ_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = packet->info.queue;
  int qid = qt->id;
  int from = 0;
  QUEUE_TYPE *nqt =  NULL;
  CONFIG *cnf = netconfig_get_conf(netconf, qid);

  try ( update_time(e, state) );
  packet->info.etime.real = e->info.time.real;
  if (cnf->nodetype != NODE_SOURCE)
    qt->finish_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  //try ( _free_packet(state, packet) );

  while ((nqt = netq_traverse_neighbor_from(state, qid, from))) {
    CHANNEL_CONF *ch = state->queuenet.edges.get_edge(&state->queuenet.edges, qid, nqt->id);
    packet->info.atime.real = e->info.time.real + ch->delay.distribution.gen(&ch->delay.distribution);
    packet->info.ctime.real = 0;
    packet->info.stime.real = 0;
    packet->info.etime.real = 0;
    packet->info.queue = nqt;
    packet->info.state = PACKET_STATE_IN;
    nq_generate_arrival_from_packet(netconf, state, packet);
    from = nqt->id + 1;
    break;
  }

  if (cnf->nodetype == NODE_SOURCE)
    nq_process_packet(netconf, state, qt);
  else
    while ((qt->is_idle(qt) && (qt->get_waiting_length(qt) > 0)))
      nq_process_packet(netconf, state, qt);

  return SUCCESS;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int nq_system_clean (NET_CONFIG *netconf, SYS_STATE_OPS *ops) {
//  NETQ_STATE *state = get_sys_state_from_ops(ops);

//TODO: Traverse all configuration to close file
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
EVENT * nq_generate_event(int type, PACKET *p, NET_CONFIG *netconf, SYS_STATE_OPS *ops) {
  NETQ_STATE *state = get_netq_state_from_ops(ops);
  EVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    //e = _generate_arrival( conf, state);
    e = NULL;
    break;
  case EVENT_END_SERVICE:
    e = nq_generate_end_service(p, netconf, state);
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
int nq_process_event (EVENT *e, NET_CONFIG *netconf, SYS_STATE_OPS *ops) {
  NETQ_STATE *state = get_netq_state_from_ops(ops);
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    nq_process_arrival(e, netconf, state);
    break;
  case EVENT_END_SERVICE:
    nq_process_end_service(e, netconf, state);
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
int nq_get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  NETQ_STATE *state = get_netq_state_from_ops(ops);
  event_list_get_first(&state->future_events, e);
  return SUCCESS;
}

/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
int nq_remove_event (SYS_STATE_OPS *ops, EVENT *e) {
  NETQ_STATE *state = get_netq_state_from_ops(ops);
  try (event_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int netq_state_init (NETQ_STATE *state, NET_CONFIG *netconf) {
  int i = 0;
  CONFIG *cnf = NULL;
  QUEUE_TYPE *qt = NULL;
  LINKED_LIST *link = NULL;
  PACKET *packet = NULL;
//  QUEUE_TYPE *fifo_queue = NULL;
  check_null_pointer(state);
  check_null_pointer(netconf);

  state->curr_time.real = 0;
  packet_list_init(&state->free_packets, LL_CONF_STORE_FREE);
  event_list_init(&state->future_events);
  measures_init (&state->measurement);

  array_setup(&state->queues, sizeof(QUEUE_TYPE), netconf->nnodes);
  graph_setup_matrix(&state->queuenet, netconf->nnodes);
  /* TODO: Build topology */
  link = netconf->channels.next;
  while (&netconf->channels != link) {
    CHANNEL_CONF *c = container_of(link, CHANNEL_CONF, list_node);
    state->queuenet.edges.set_edge(&state->queuenet.edges, c->src, c->dest, c);
    link = link->next;
  }

  for (i=0; i< netconf->nnodes; i++) {
    qt = array_get(&state->queues, i);
    qt->id = i;
    cnf = netconfig_get_conf(netconf, i);
    state->queuenet.nodes.set_node(&state->queuenet.nodes, i, qt);

    /* TODO: Setup initial events */
    if (cnf->nodetype == NODE_SOURCE) {
      try (_new_packet(state, &packet));
      packet_init(packet);
      packet->info.atime.real = state->curr_time.real;
      packet->info.stime.real = state->curr_time.real;
      packet->info.queue = qt;
      packet->info.state = PACKET_STATE_PROCESSING;
      nq_generate_end_service(packet, netconf, state);
    } else if (cnf->nodetype == NODE_SINK) {
      cnf->queue_conf.num_servers = 0;
      cnf->queue_conf.max_waiters = 0;
    }
    fifo_setup(qt, cnf->queue_conf.num_servers, cnf->queue_conf.max_waiters);
  }

  state->ops.allow_continue = nq_allow_continue;
  state->ops.clean = nq_system_clean;
  state->ops.generate_event = nq_generate_event;
  state->ops.get_next_event = nq_get_next_event;
  state->ops.process_event = nq_process_event;
  state->ops.remove_event = nq_remove_event;
  return SUCCESS;
}

int netq_run(char *f) {
  extern NET_CONFIG netconf;
  int i = 0;
  NETQ_STATE state;
  SYS_STATE_OPS *ops = NULL;
  time_t start;

  start = time(NULL);

  netconfig_init(&netconf, 4);
  netconfig_parse_nodes("src/netsim/conf/source.conf");
  netconfig_parse_nodes("src/netsim/conf/node1.conf");
  netconfig_parse_nodes("src/netsim/conf/node2.conf");
  netconfig_parse_nodes("src/netsim/conf/sink.conf");
  netconfig_parse_channels("src/netsim/conf/netconf.conf");
  netq_state_init(&state, &netconf);
  ops = &state.ops;
  pisas_sched(&netconf, ops);
  printf("ALL SYSTEM\n");
  print_measurement(&state.measurement);
  for (i=0; i<netconf.nnodes; i++) {
    QUEUE_TYPE *qt = netq_get_queue(&state, i);
    printf("Queue %d \n", i);
    print_measurement(&((FIFO_QINFO*)qt->info)->measurement);
  }
  netsim_print_theorical_mm1 (80, 100);
  printf("Time of simulation: %d (seconds) \n", time(NULL)-start);
  return SUCCESS;
}
