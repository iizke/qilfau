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
int oneq_new_packet (ONEQ_STATE *state, JOB **p) {
  try ( job_list_new_job(&state->free_packets, p) );
  (*p)->queue = state->queues.curr_queue;
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
int oneq_free_packet (ONEQ_STATE *state, JOB *p) {
  try (linked_list_remove(&p->list_node));
  job_list_remove_job(&state->free_packets, p);
  return SUCCESS;
}

/**
 * Update system time
 * @param e : Event
 * @param state : System state
 * @return Error code (see more in def.h and error.h)
 */
int oneq_update_time (ONEQ_STATE *state, double time) {
  check_null_pointer(state);
  if (state->curr_time <= time)
    state->curr_time = time;
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
SEVENT* oneq_generate_arrival (ONEQ_STATE *state, SCHED_CONFIG *conf) {
  SEVENT *e = NULL;
  int error_code = SUCCESS;
  sevent_list_new_event(&state->future_events, &e);
  e->type = EVENT_ARRIVAL;
  error_code = sevent_setup(e, &conf->arrival_conf, state->curr_time);
  if (error_code < 0) {
    free_gc (e);
    return NULL;
  }
  sevent_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Generate new end-service event.
 * @param p : Processing packet (for this event)
 * @param conf : user configuration
 * @param state : System state
 * @return New event (already added in the event list)
 */
SEVENT* oneq_generate_end_service (ONEQ_STATE *state, JOB *p, SCHED_CONFIG *conf) {
  SEVENT *e = NULL;
  sevent_list_new_event(&state->future_events, &e);
  e->type = EVENT_END_SERVICE;
  sevent_setup(e, &conf->service_conf, state->curr_time);
  event_get_event_info(e)->packet = p;
  sevent_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
static int _allow_continue (SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);

  STOP_SCONF *stop_conf = &conf->stop_conf;
  if ((stop_conf->max_time > 0 && state->curr_time > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival) ||
      ((conf->arrival_conf.type == RANDOM_FILE) && (conf->arrival_conf.from_file) && (feof(conf->arrival_conf.from_file)))) {
    if (conf->stop_conf.queue_zero == STOP_QUEUE_ZERO) {
      SQUEUE_TYPE *qt = NULL;
      qt = state->queues.curr_queue;
      if (qt->get_waiting_length(qt) > 0)
        return 1;
    } else {
      conf->arrival_conf.type = RANDOM_OTHER;
      conf->arrival_conf.distribution.gen = NULL;
      if (!sevent_list_is_empty(&state->future_events))
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
static int _process_packet (SCHED_CONFIG *conf, ONEQ_STATE *state) {
  SQUEUE_TYPE *qt = state->queues.curr_queue;
  SEVENT *e = NULL;
  JOB *p = NULL;

  qt->select_waiting_packet(qt, &p);
  p->stime = state->curr_time;
  qt->process_packet(qt, p);
  smeasurement_collect_data(&state->measurement, p, state->curr_time);
  e = oneq_generate_end_service (state, p, conf);
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
static int _packet_from_event (SEVENT *e, JOB *p) {
  switch (e->type) {
  case EVENT_ARRIVAL:
    p->atime = e->time;
    p->state = JOB_STATE_IN;
    //e->data = p;
    break;
  case EVENT_END_SERVICE:
    p->etime = e->time;
    p->state = JOB_STATE_OUT;
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
static int _process_arrival (SEVENT *e, SCHED_CONFIG *conf, ONEQ_STATE *state) {
  JOB *packet = NULL;
  SQUEUE_TYPE *qt = state->queues.curr_queue;

  try ( oneq_update_time(state, e->time) );
  try ( oneq_new_packet(state, &packet) );
  _packet_from_event (e, packet);
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->state == JOB_STATE_DROPPED)
    oneq_free_packet(state, packet);

  oneq_generate_arrival(state, conf);

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
static int _process_end_service (SEVENT *e, SCHED_CONFIG *conf, ONEQ_STATE *state) {
  JOB *packet = event_get_event_info(e)->packet;
  SQUEUE_TYPE *qt = state->queues.curr_queue;

  try ( oneq_update_time(state, e->time) );
  _packet_from_event(e, packet);
  qt->finish_packet(qt, packet);
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( oneq_free_packet(state, packet) );

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
static int _system_clean (SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
  //ONEQ_STATE *state = get_sys_state_from_ops(ops);

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  //if (state->departure_file)
  //fclose(state->departure_file);
  if (conf->queue_conf.out_file)
    fclose(conf->queue_conf.out_file);

  //This part of code is supported by Garbage collector -> comment these codes
//  while (state->free_packets.size > 0) {
//    // free packet
//    JOB *p = NULL;
//    job_list_get_first(&state->free_packets, &p);
//    linked_list_remove(&p->list_node);
//    free(p);
//  }
//  free(state->queues.curr_queue);
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
static SEVENT * _generate_event(int type, JOB *p, SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);
  SEVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    e = oneq_generate_arrival(state, conf);
    break;
  case EVENT_END_SERVICE:
    e = oneq_generate_end_service(state, p, conf);
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
static int _process_event (SEVENT *e, SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);
  switch (e->type) {
  case EVENT_ARRIVAL:
    _process_arrival(e, conf, state);
    break;
  case EVENT_END_SERVICE:
    _process_end_service(e, conf, state);
    break;
  default:
    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", e->type);
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
static int _get_next_event (SCHED_STATE_OPS *ops, SEVENT **e) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);
  sevent_list_get_first(&state->future_events, e);
  return SUCCESS;
}

/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
static int _remove_event (SCHED_STATE_OPS *ops, SEVENT *e) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);
  try (sevent_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

int oneq_set_ops (ONEQ_STATE *state) {
  check_null_pointer(state);
  state->ops.allow_continue = _allow_continue;
  state->ops.clean = _system_clean;
  state->ops.generate_event = _generate_event;
  state->ops.get_next_event = _get_next_event;
  state->ops.process_event = _process_event;
  state->ops.remove_event = _remove_event;
  return SUCCESS;
}
/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int sched_sys_state_init (ONEQ_STATE *state, SCHED_CONFIG *conf) {
  SQUEUE_TYPE *fifo_queue = NULL;
  check_null_pointer(state);

  state->curr_time = 0;
  job_list_init(&state->free_packets, LL_CONF_STORE_FREE);
  sevent_list_init(&state->future_events);
  smeasures_init (&state->measurement);
  squeue_man_init(&state->queues);

  sched_fifo_init(&fifo_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
  squeue_man_register_new_type(&state->queues, fifo_queue);
  state->queues.curr_queue = fifo_queue;
  //printf("Config to use queue %d \n", conf.queue_conf.type);
  squeue_man_activate_type(&state->queues, conf->queue_conf.type);

  oneq_set_ops(state);

  return SUCCESS;
}
