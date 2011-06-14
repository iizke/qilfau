/**
 * @file csma.c
 * Model of CSMA system (Carrier Sense Multiple Access)
 *
 * @date Created on: May 16, 2011
 * @author iizke
 */

#include <stdlib.h>
#include "conf/config.h"
#include "error.h"
#include "queues/fifo.h"
#include "random.h"
#include "csma.h"

#define queue_state(qm) (((FIFO_QINFO*)(qm->curr_queue->info))->state)

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
    iprint(LEVEL_ERROR, "Event is late, may be FES is not sorted \n");
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
  int error_code = SUCCESS;

  //iprint(LEVEL_ERROR, "Gen Arrival Event\n");

  if (event_list_new_event(&state->future_events, &e) < 0)
    return NULL;
  e->info.type = EVENT_ARRIVAL;
  error_code = event_setup(e, &conf->arrival_conf, state->curr_time);
  if (error_code < 0) {
    free_gc (e);
    return NULL;
  }
  csma_new_packet(state, &packet);
  if (!packet) {
    csma_remove_event(&state->ops, e);
    iprint(LEVEL_WARNING, "Cannot allocate new packet \n");
    return NULL;
  }
  e->info.packet = packet;
  packet->info.atime.real = e->info.time.real;
  packet->info.state = PACKET_STATE_IN;
  packet->info.queue = NULL;
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

  if (!p) {
    iprint(LEVEL_ERROR, "BUG! Cannot generate end-service event because packet is null \n");
    return NULL;
  }
  if (event_list_new_event(&state->future_events, &e) < 0)
    return NULL;
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
  if (!p) {
    iprint(LEVEL_ERROR, "BUG! Queue is persistent but no waiting packet !\n");
    //exit(1);
  }
  if (event_list_new_event(&state->future_events, &e) < 0)
    return NULL;
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
    QUEUE_TYPE *qt = p->info.queue;
    if (qt->get_waiting_length(qt) == 0) {
      iprint(LEVEL_ERROR, "BUG! Generate access with persistent but no waiting packet \n");
    } else {
      ((FIFO_QINFO*)(p->info.queue->info))->state = QUEUE_STATE_PERSISTENT;
      if (p->info.ctime.real == 0)
        p->info.ctime.real = state->curr_time.real;
    }
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
  e->info.packet = NULL;
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
  if (p == NULL) {
    iprint(LEVEL_ERROR, "BUG! There is no waiting packet but still have to process it\n");
    return ERR_POINTER_NULL;
  }

  state->channel_state = CHANNEL_BUSY;
  p->info.stime.real = state->curr_time.real;
  ((FIFO_QINFO*)(qt->info))->state = QUEUE_STATE_TRANSFERING;
  qt->process_packet(qt, p);
  measurement_collect_data(&state->measurement, p, state->curr_time);
  e = csma_generate_end_service (p, conf, state);
  if (!e) {
    iprint(LEVEL_WARNING, "Cannot generate end service event \n");
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
  csma_update_time(e, state);
  if (state->channel_state == CHANNEL_BUSY)
    csma_generate_access(e->info.packet, conf, state);
  else {
    /// If channel is idle/free then occuppy the channel
    csma_process_a_packet(e->info.packet->info.queue, conf, state);
  }
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

  csma_update_time(e, state);
  // generate next arrival event
  csma_generate_arrival(conf, state);

  measurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->info.state == PACKET_STATE_DROPPED) {
    csma_free_packet(state, packet);
    return SUCCESS;
  }

  if ((qt->get_waiting_length(qt) == 1) && (qt->is_idle(qt))) {
    // process packet
    //e->info.type = EVENT_ACCESS_CHANNEL;
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
    QUEUE_TYPE *qt = state->queues[i].curr_queue;
    if (((FIFO_QINFO*)qt->info)->state == QUEUE_STATE_PERSISTENT) {
      if (qt->get_waiting_length(qt) <= 0) {
        iprint(LEVEL_ERROR, "BUG! Queue in state Persistent while no waiting packet \n");
        ((FIFO_QINFO*)qt->info)->state = QUEUE_STATE_NOPERSISTENT;
        continue;
        //exit(1);
        //return ERR_CSMA_INCONSISTENT;
      }
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

  csma_update_time(e, state);

  state->channel_state = CHANNEL_FREE;
  persistent_count = csma_count_persistent_queue(conf, state, &qt);
    // if the station still has waiting packets, then ...
  if (persistent_count > 1){
    // now is collision again -> create collision event to all  persistent queues
    int i = 0;
    EVENT *collsion = csma_generate_collision(NULL, conf, state);
    double collision_time = collsion->info.time.real - state->curr_time.real;
    state->channel_state = CHANNEL_BUSY;

    for (i=0; i < conf->csma_conf.nstations; i++) {
      qt = state->queues[i].curr_queue;
      packet = NULL;
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

  csma_update_time(e, state);
  state->channel_state = CHANNEL_FREE;
  qt->finish_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( csma_free_packet(state, packet) );
  ((FIFO_QINFO*)qt->info)->state = QUEUE_STATE_NOPERSISTENT;
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
int csma_get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  CSMA_STATE *state = get_csma_state_from_ops(ops);
  event_list_get_first(&state->future_events, e);
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

  if ((stop_conf->max_time > 0 && state->curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival) ||
      ((conf->arrival_conf.from_file) && (feof(conf->arrival_conf.from_file))) ) {
    if (conf->stop_conf.queue_zero == STOP_QUEUE_ZERO) {
      int i = 0;
      QUEUE_TYPE *qt = NULL;
      for (i=0; i<state->nqueues;i++) {
        qt = state->queues[i].curr_queue;
        if (qt->get_waiting_length(qt) > 0)
          return 1;
      }
    } else {
      // stop generating arrival-event
      conf->arrival_conf.type = RANDOM_OTHER;
      if (!event_list_is_empty(&state->future_events))
        return 1;
    }
    return 0;
  }
  return 1;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int csma_system_clean (CONFIG *conf, SYS_STATE_OPS *ops) {
  //CSMA_STATE *state = get_csma_state_from_ops(ops);

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  if (conf->queue_conf.out_file)
    fclose(conf->queue_conf.out_file);

  //This part of code is supported by Garbage collector -> comment these codes
//  while (state->free_packets.size > 0) {
//    PACKET *p = NULL;
//    packet_list_get_first(&state->free_packets, &p);
//    linked_list_remove(&p->list_node);
//    free(p);
//  }
//
//  free(state->queues[0].curr_queue);
//  free(state->queues);
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

  state->queues = malloc_gc(sizeof(QUEUE_MAN) * conf->csma_conf.nstations);
  state->nqueues = conf->csma_conf.nstations;
  if (!state->queues)
    return ERR_MALLOC_FAIL;

  ffq = malloc_gc (sizeof(QUEUE_TYPE) * state->nqueues);
  if (!ffq)
    return ERR_MALLOC_FAIL;

  for (i = 0; i < conf->csma_conf.nstations; i++) {
    queue_man_init(&state->queues[i]);
    fifo_setup(&ffq[i], conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
    queue_man_register_new_type(&state->queues[i], &ffq[i]);
    state->queues[i].curr_queue = &ffq[i];
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
