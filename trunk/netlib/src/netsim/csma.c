/**
 * @file csma.c
 * Model of CSMA system (Carrier Sense Multiple Access)
 *
 * @date Created on: May 16, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "conf/config.h"
#include "../error.h"
#include "../queues/fifo.h"
#include "../random.h"
#include "csma.h"

#define get_csma_state_from_ops(_ops) (container_of(_ops, CSMA_STATE, ops))
#define queue_state(qm) (((FIFO_QINFO*)(qm->curr_queue->info))->state)

/**
 * Update system time.
 * @param e : Event
 * @param state : system state
 * @return Error code (more in def.h and error.h)
 */
static int csma_update_time (EVENT *e, CSMA_STATE *state) {
  check_null_pointer(e);
  check_null_pointer(state);
  if (state->curr_time.real <= e->info.time.real)
    state->curr_time.real = e->info.time.real;
  else {
    iprintf(LEVEL_WARNING, "Event is late, may be FES is not sorted \n");
    return ERR_EVENT_TIME_WRONG;
  }
  return SUCCESS;
}

/**
 * Create new packet
 * @param state : System state
 * @param p : new packet
 * @return Error code (see more in def.h and error.h)
 */
static int csma_new_packet (CSMA_STATE *state, PACKET **p) {
  try ( packet_list_new_packet(&state->free_packets, p) );
  return SUCCESS;
}
/**
 * Free a packet. This action is controlled by program to have a good performance
 * by avoiding free/malloc operations frequently
 *
 * @param state : system state
 * @param p : packet
 * @return Error code (see more in def.h and error.h)
 */
static int csma_free_packet (CSMA_STATE *state, PACKET *p) {
  try (linked_list_remove(&p->list_node));
  packet_list_remove_packet(&state->free_packets, p);
  return SUCCESS;
}
/**
 * Generate arrival event
 * @param conf : user configuration
 * @param state : system state
 * @return new event (already added in Event list)
 */
EVENT* csma_generate_arrival(CONFIG *conf, CSMA_STATE *state) {
  EVENT *e = NULL;
  PACKET *packet = NULL;

  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_ARRIVAL;
  event_setup(e, &conf->arrival_conf, state->curr_time);
  csma_new_packet(state, &packet);
  e->info.packet = packet;
  packet->info.atime.real = e->info.time.real;
  packet->info.state = PACKET_STATE_IN;
  event_list_insert_event(&state->future_events, e);
  return e;
}
/**
 * Generate new end-service event.
 * @param p : Processing packet (for this event)
 * @param conf : user configuration
 * @param state : System state
 * @return New event (already added in the event list)
 */
EVENT * csma_generate_end_service(PACKET *p, CONFIG *conf, CSMA_STATE *state) {
  EVENT *e = NULL;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_END_SERVICE;
  e->info.packet = p;
  event_setup(e, &conf->service_conf, state->curr_time);
  p->info.etime.real = e->info.time.real;
  ((FIFO_QINFO*)p->info.queue->info)->state = QUEUE_STATE_TRANSFERING;
  event_list_insert_event(&state->future_events, e);
  state->channel_state = CHANNEL_BUSY;
  return e;
}
/**
 * Generate new tentative access event.
 * @param p : Processing packet (for this event)
 * @param collision_time : Time to detect the collision
 * @param conf : user configuration
 * @param state : System state
 * @return New event (already added in the event list)
 */
static EVENT * csma_generate_access_nopersistent (PACKET *p, double collision_time, CONFIG *conf, CSMA_STATE *state) {
  EVENT *e = NULL;
  int backoff = 0;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_ACCESS_CHANNEL;
  e->info.packet = p;
  backoff = random_dist_gen((&conf->csma_conf.backoff_conf.distribution));
  ((FIFO_QINFO*)(p->info.queue->info))->state = QUEUE_STATE_NOPERSISTENT;
  e->info.time.real = state->curr_time.real + backoff * conf->csma_conf.slot_time + collision_time;
  if (p->info.ctime.real == 0)
    p->info.ctime.real = e->info.time.real;

  event_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Generate an access event (if necessary) or change to persistent-mode
 * @param p : packet
 * @param conf : user configuration
 * @param state : system state
 * @return New event (already added in the event list)
 */
EVENT * csma_generate_access(PACKET *p, CONFIG *conf, CSMA_STATE *state) {
  int persistent = 0;
  EVENT *e = NULL;
  persistent = random_dist_gen(&conf->csma_conf.persistent_conf.distribution);
  //persistent = (&(conf->csma_conf.persistent_conf.distribution))->gen( &(conf->csma_conf.persistent_conf.distribution) );
  if (persistent == 0)
    e = csma_generate_access_nopersistent(p, 0, conf, state);
  else {
    ((FIFO_QINFO*)(p->info.queue->info))->state = QUEUE_STATE_PERSISTENT;
    if (p->info.ctime.real == 0)
      p->info.ctime.real = state->curr_time.real;
  }

  return e;
}

/**
 * Generate collision event
 * @param p : packet but should be NULL (no use)
 * @param conf : user configuration
 * @param state : system state
 * @return New event (already added in the event list)
 */
EVENT * csma_generate_collision(PACKET *p, CONFIG *conf, CSMA_STATE *state) {
  EVENT *e = NULL;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_END_COLLISION;
  e->info.packet = p;
  e->info.time.real = state->curr_time.real + conf->csma_conf.collision_time;
  event_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Process a packet in a queue
 * @param qt : a considered queue
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
static int csma_process_a_packet (QUEUE_TYPE *qt, CONFIG *conf, CSMA_STATE *state) {
  EVENT *e = NULL;
  PACKET *p = NULL;
  qt->select_waiting_packet(qt, &p);
  p->info.stime.real = state->curr_time.real;
  qt->process_packet(qt, p);
  measurement_collect_data(&state->measurement, p, state->curr_time);
  e = csma_generate_end_service (p, conf, state);
  if (!e) {
    iprintf(LEVEL_WARNING, "Cannot generate end service event \n");
    return ERR_POINTER_NULL;
  }
  return SUCCESS;
}

/**
 * Process Access event
 * @param e : Event
 * @param conf : configuration from user
 * @param state : system state
 * @return Error code (see more in def.h and libs/error.h)
 */
int csma_process_access_event(EVENT *e, CONFIG *conf, CSMA_STATE *state) {
  if (state->channel_state == CHANNEL_BUSY)
    csma_generate_access(e->info.packet, conf, state);
  else
    /// If channel is idle/free then occuppy the channel
    csma_process_a_packet(e->info.packet->info.queue, conf, state);

  return SUCCESS;
}

/**
 * Process Arrival event
 * @param e : Event
 * @param conf : configuration from user
 * @param state : system state
 * @return Error code (see more in def.h and libs/error.h)
 */
int csma_process_arrival(EVENT *e, CONFIG *conf, CSMA_STATE *state) {
  PACKET *packet = e->info.packet;
  int station = irand_gen_int_uniform(0, conf->csma_conf.nstations - 1);
  QUEUE_TYPE *qt = state->queues[station].curr_queue;

  try ( csma_update_time(e, state) );
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);

  // generate next arrival event
  csma_generate_arrival(conf, state);

  if (qt->is_idle(qt)) {
    // process packet
    e->info.type = EVENT_ACCESS_CHANNEL;
    csma_process_access_event(e, conf, state);
  }
  return SUCCESS;
}

/**
 * Count number of persistent queues/stations and return one pointer to persistent queue
 * @param conf : user configuration
 * @param state : system state
 * @param persistent_q : pointer to a persistent queue
 * @return Error code (see more in def.h and error.h)
 */
static int csma_count_persistent_queue(CONFIG *conf, CSMA_STATE *state, QUEUE_TYPE **persistent_q) {
  int i = 0;
  int sum = 0;
  for (i = 0; i < conf->csma_conf.nstations; i++) {
    if (((FIFO_QINFO*)state->queues[i].curr_queue->info)->state == QUEUE_STATE_PERSISTENT) {
      sum++;
      *persistent_q = state->queues[i].curr_queue;
    }
  }
  return sum;
}

/**
 * Process Collision event
 * @param e : Event
 * @param conf : configuration from user
 * @param state : system state
 * @return Error code (see more in def.h and libs/error.h)
 */
int csma_process_collision(EVENT *e, CONFIG *conf, CSMA_STATE *state) {
  QUEUE_TYPE *qt = NULL;
  int persistent_count = 0;
  PACKET *packet = NULL;

  state->channel_state = CHANNEL_FREE;
  persistent_count = csma_count_persistent_queue(conf, state, &qt);
    // if the station still has waiting packets, then ...
  if (persistent_count > 1){
    // now is collision -> create collision event to all  persistent queues
    int i = 0;
    EVENT *collsion = csma_generate_collision(NULL, conf, state);
    double collision_time = collsion->info.time.real - state->curr_time.real;
    state->channel_state = CHANNEL_BUSY;

    for (i=0; i < conf->csma_conf.nstations; i++) {
      qt = state->queues[i].curr_queue;
      if (queue_state((&state->queues[i])) == QUEUE_STATE_PERSISTENT) {
        queue_state((&state->queues[i])) = QUEUE_STATE_NOPERSISTENT;
        qt->get_waiting_packet(qt, &packet);
        //csma_generate_collision(packet, conf, state);
        csma_generate_access_nopersistent(packet, collision_time, conf, state);
      }
    }
  }else if (persistent_count == 1) {
      csma_process_a_packet(qt, conf, state);
  }

  return SUCCESS;
}

/**
 * Process End-service event
 * @param e : Event
 * @param conf : configuration from user
 * @param state : system state
 * @return Error code (see more in def.h and libs/error.h)
 */
int csma_process_end_service(EVENT *e, CONFIG *conf, CSMA_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = packet->info.queue;

  state->channel_state = CHANNEL_FREE;
  qt->finish_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( csma_free_packet(state, packet) );
  if (qt->is_idle(qt) && (qt->get_waiting_length(qt) > 0))
    ((FIFO_QINFO*)qt->info)->state = QUEUE_STATE_PERSISTENT;

  csma_process_collision(NULL, conf, state);
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
EVENT * csma_generate_event(int type, PACKET *p, CONFIG *conf, SYS_STATE_OPS *ops) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  EVENT *e = NULL;
  switch (type) {
  case EVENT_ARRIVAL:
    e = csma_generate_arrival(conf, state);
    break;
  case EVENT_END_SERVICE:
    e = csma_generate_end_service(p, conf, state);
    break;
  case EVENT_ACCESS_CHANNEL:
    e = csma_generate_access(p, conf, state);
    break;
  case EVENT_END_COLLISION:
    e = csma_generate_collision(p, conf, state);
    break;
  default:
    iprintf(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", type);
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
int csma_process_event (EVENT *e, CONFIG *conf, SYS_STATE_OPS *ops) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    csma_process_arrival(e, conf, state);
    break;
  case EVENT_END_SERVICE:
    csma_process_end_service(e, conf, state);
    break;
  case EVENT_ACCESS_CHANNEL:
    csma_process_access_event (e, conf, state);
    break;
  case EVENT_END_COLLISION:
    csma_process_collision(e, conf, state);
    break;
  default:
    iprintf(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", e->info.type);
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
int csma_get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  event_list_get_first(&state->future_events, e);
  return SUCCESS;
}
/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
int csma_remove_event (SYS_STATE_OPS *ops, EVENT *e) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  try (event_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
int csma_allow_continue (CONFIG *conf, SYS_STATE_OPS *ops) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);

  STOP_CONF *stop_conf = &conf->stop_conf;
  if (stop_conf->max_time > 0 && state->curr_time.real > stop_conf->max_time)
    return 0;
  if (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival)
    return 0;
  if ((conf->arrival_conf.from_file) && (feof(conf->arrival_conf.from_file)))
    return 0;
  return 1;
}
/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int csma_system_clean (CONFIG *conf, SYS_STATE_OPS *ops) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  int i = 0;

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  if (conf->queue_conf.out_file)
      fclose(conf->queue_conf.out_file);

  while (state->free_packets.size > 0) {
    PACKET *p = NULL;
    packet_list_get_first(&state->free_packets, &p);
    linked_list_remove(&p->list_node);
    free(p);
  }

  for (i = 0; i < conf->csma_conf.nstations; i++) {
    free(state->queues[i].curr_queue);
  }
  free(state->queues);
  return SUCCESS;
}
/**
 * Initialize CSMA system state
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int csma_state_init (CSMA_STATE *state, CONFIG *conf) {
  QUEUE_TYPE *ffq = NULL;
  int i = 0;
  check_null_pointer(state);

  state->curr_time.real = 0;
  state->channel_state = CHANNEL_FREE;
  packet_list_init(&state->free_packets, LL_CONF_STORE_FREE);
  event_list_init(&state->future_events);
  measures_init (&state->measurement);

  state->queues = malloc(sizeof(QUEUE_MAN) * conf->csma_conf.nstations);
  state->nqueues = conf->csma_conf.nstations;
  if (!state->queues)
    return ERR_MALLOC_FAIL;

  for (i = 0; i < conf->csma_conf.nstations; i++) {
    ffq = NULL;
    queue_man_init(&state->queues[i]);
    fifo_init(&ffq, conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
    queue_man_register_new_type(&state->queues[i], ffq);
    state->queues[i].curr_queue = ffq;
    queue_man_activate_type(&state->queues[i], conf->queue_conf.type);
  }

  //printf("Config to use queue %d \n", conf.queue_conf.type);
  state->ops.generate_event = csma_generate_event;
  state->ops.process_event = csma_process_event;
  state->ops.remove_event = csma_remove_event;
  state->ops.get_next_event = csma_get_next_event;
  state->ops.allow_continue = csma_allow_continue;
  state->ops.clean = csma_system_clean;
  return SUCCESS;
}
