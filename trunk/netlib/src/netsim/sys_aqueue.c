/**
 * @file sys_aqueue.c
 * Simulation of system with one queue (but can be multi-servers)
 *
 * @date Created on: May 19, 2011
 * @author iizke
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "queues/fifo.h"
#include "sys_aqueue.h"

/**
 * Create new packet
 * @param state : System state
 * @param p : new packet
 * @return Error code (see more in def.h and error.h)
 */
static int _new_packet (SYS_STATE *state, PACKET **p) {
  try ( packet_list_new_packet(&state->free_packets, p) );
  (*p)->info.queue = state->queues.curr_queue;
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
static int _free_packet (SYS_STATE *state, PACKET *p) {
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
static int update_time (EVENT *e, SYS_STATE *state) {
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

/**
 * Generate arrival event
 * @param conf : user configuration
 * @param state : system state
 * @return new event (already added in Event list)
 */
EVENT* _generate_arrival (CONFIG *conf, SYS_STATE *state) {
  EVENT *e = NULL;
  int error_code = SUCCESS;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_ARRIVAL;
  error_code = event_setup(e, &conf->arrival_conf, state->curr_time);
  if (error_code < 0) {
    free (e);
    return NULL;
  }
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
EVENT* _generate_end_service (PACKET *p, CONFIG *conf, SYS_STATE *state) {
  EVENT *e = NULL;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_END_SERVICE;
  event_setup(e, &conf->service_conf, state->curr_time);
  e->info.packet = p;
  event_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
int _allow_continue (CONFIG *conf, SYS_STATE_OPS *ops) {
  SYS_STATE *state = get_sys_state_from_ops(ops);

  STOP_CONF *stop_conf = &conf->stop_conf;
  if ((stop_conf->max_time > 0 && state->curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival) ||
      ((conf->arrival_conf.from_file) && (feof(conf->arrival_conf.from_file)))) {
    if (conf->stop_conf.queue_zero == STOP_QUEUE_ZERO) {
      QUEUE_TYPE *qt = NULL;
      qt = state->queues.curr_queue;
      if (qt->get_waiting_length(qt) > 0)
        return 1;
    } else {
      conf->arrival_conf.type = RANDOM_OTHER;
      if (!event_list_is_empty(&state->future_events))
        return 1;
    }
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
static int _process_packet (CONFIG *conf, SYS_STATE *state) {
  QUEUE_TYPE *qt = state->queues.curr_queue;
  EVENT *e = NULL;
  PACKET *p = NULL;

  qt->select_waiting_packet(qt, &p);
  p->info.stime.real = state->curr_time.real;
  qt->process_packet(qt, p);
  measurement_collect_data(&state->measurement, p, state->curr_time);
  e = _generate_end_service (p, conf, state);
  if (!e) {
    iprint(LEVEL_WARNING, "Cannot generate end service event \n");
    return ERR_POINTER_NULL;
  }
  return SUCCESS;
}

/**
 * Setup time and type of packet extracted from an event
 * @param e : Event
 * @param p : Packet
 * @return Error code (see more in def.h and error.h)
 */
int _packet_from_event (EVENT *e, PACKET *p) {
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    p->info.atime.real = e->info.time.real;
    p->info.state = PACKET_STATE_IN;
    e->info.packet = p;
    break;
  case EVENT_END_SERVICE:
    p->info.etime.real = e->info.time.real;
    p->info.state = PACKET_STATE_OUT;
    break;
  default:
    iprint(LEVEL_WARNING, "Event Type not supported \n");
    return ERR_EVENT_TYPE_FAIL;
    break;
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
int _process_arrival (EVENT *e, CONFIG *conf, SYS_STATE *state) {
  PACKET *packet = NULL;
  QUEUE_TYPE *qt = state->queues.curr_queue;

  try ( update_time(e, state) );
  try ( _new_packet(state, &packet) );
  _packet_from_event (e, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->info.state == PACKET_STATE_DROPPED)
    _free_packet(state, packet);

  _generate_arrival(conf, state);

  if ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) >= 1))
    _process_packet(conf, state);

  return SUCCESS;
}

/**
 * Process end-service event
 * @param e : event
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
int _process_end_service (EVENT *e, CONFIG *conf, SYS_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = state->queues.curr_queue;

  try ( update_time(e, state) );
  _packet_from_event(e, packet);
  qt->finish_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( _free_packet(state, packet) );

  while (qt->is_idle(qt) && (qt->get_waiting_length(qt) > 0))
    _process_packet(conf, state);

  return SUCCESS;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int _system_clean (CONFIG *conf, SYS_STATE_OPS *ops) {
  SYS_STATE *state = get_sys_state_from_ops(ops);

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  //if (state->departure_file)
  //fclose(state->departure_file);
  if (conf->queue_conf.out_file)
      fclose(conf->queue_conf.out_file);

  while (state->free_packets.size > 0) {
    // free packet
    PACKET *p = NULL;
    packet_list_get_first(&state->free_packets, &p);
    linked_list_remove(&p->list_node);
    free(p);
  }
  free(state->queues.curr_queue);
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
EVENT * _generate_event(int type, PACKET *p, CONFIG *conf, SYS_STATE_OPS *ops) {
  SYS_STATE *state = get_sys_state_from_ops(ops);
  EVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    e = _generate_arrival( conf, state);
    break;
  case EVENT_END_SERVICE:
    e = _generate_end_service(p, conf, state);
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
int _process_event (EVENT *e, CONFIG *conf, SYS_STATE_OPS *ops) {
  SYS_STATE *state = get_sys_state_from_ops(ops);
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    _process_arrival(e, conf, state);
    break;
  case EVENT_END_SERVICE:
    _process_end_service(e, conf, state);
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
int _get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  SYS_STATE *state = get_sys_state_from_ops(ops);
  event_list_get_first(&state->future_events, e);
  return SUCCESS;
}

/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
int _remove_event (SYS_STATE_OPS *ops, EVENT *e) {
  SYS_STATE *state = get_sys_state_from_ops(ops);
  try (event_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int sys_state_init (SYS_STATE *state, CONFIG *conf) {
  QUEUE_TYPE *fifo_queue = NULL;
  check_null_pointer(state);

  state->curr_time.real = 0;
  packet_list_init(&state->free_packets, LL_CONF_STORE_FREE);
  event_list_init(&state->future_events);
  measures_init (&state->measurement);
  queue_man_init(&state->queues);

  fifo_init(&fifo_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
  queue_man_register_new_type(&state->queues, fifo_queue);
  state->queues.curr_queue = fifo_queue;
  //printf("Config to use queue %d \n", conf.queue_conf.type);
  queue_man_activate_type(&state->queues, conf->queue_conf.type);

  state->ops.allow_continue = _allow_continue;
  state->ops.clean = _system_clean;
  state->ops.generate_event = _generate_event;
  state->ops.get_next_event = _get_next_event;
  state->ops.process_event = _process_event;
  state->ops.remove_event = _remove_event;
  return SUCCESS;
}
