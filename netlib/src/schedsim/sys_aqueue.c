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

SEVENT* oneq_generate_event (ONEQ_STATE *state, int type, RANDOM_SCONF *conf, JOB*job) {
  SEVENT *e = NULL;
  SEVENT_ONEQ *ex = NULL;
  int error_code = SUCCESS;
  sevent_list_new_event_info(&state->future_events, sizeof(SEVENT_ONEQ), &e);
  ex = event_oneq_from_event(e);

  e->type = type;
  error_code = sevent_setup(e, conf, state->curr_time);
//  if (error_code < 0) {
//    free_gc (e);
//    return NULL;
//  }
  if (!job) { // job is null, mean that it is arrival job
    oneq_new_packet(state, &job);
    job->atime = e->time;
    job->state = JOB_STATE_IN;
  }
  ex->packet = job;
  if (error_code == SUCCESS)
    sevent_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
static int _allow_continue (SCHED_STATE_OPS* ops, SCHED_CONFIG *conf) {
  ONEQ_STATE *state = get_oneq_state_from_ops(ops);

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
static int _process_packet (ONEQ_STATE *state, SCHED_CONFIG *conf) {
  SQUEUE_TYPE *qt = state->queues.curr_queue;
  SEVENT *e = NULL;
  JOB *p = NULL;

  qt->select_waiting_packet(qt, &p);
  p->stime = state->curr_time;
  qt->process_packet(qt, p);
  smeasurement_collect_data(&state->measurement, p, state->curr_time);
  e = oneq_generate_event(state, EVENT_END_SERVICE, &conf->service_conf, p);
  if (!e) {
    iprint(LEVEL_WARNING, "Cannot generate end service event \n");
    return ERR_POINTER_NULL;
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
static int _process_arrival (SEVENT *e, ONEQ_STATE *state, SCHED_CONFIG *conf) {
  check_null_pointer(e);
  SEVENT_ONEQ *ex = event_oneq_from_event(e);
  JOB *packet = ex->packet;
  SQUEUE_TYPE *qt = packet->queue;

  if (e->type != EVENT_ARRIVAL)
    return ERR_EVENT_TYPE_FAIL;

  try ( oneq_update_time(state, e->time) );
  packet->atime = e->time;
  packet->state = JOB_STATE_IN;
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->state == JOB_STATE_DROPPED)
    oneq_free_packet(state, packet);

  //oneq_generate_arrival(state, conf);
  oneq_generate_event(state, EVENT_ARRIVAL, &conf->arrival_conf, NULL);

  while ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) >= 1))
    _process_packet(state, conf);

  return SUCCESS;
}

/**
 * Process end-service event
 * @param e : event
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
static int _process_end_service (SEVENT *e, ONEQ_STATE *state, SCHED_CONFIG *conf) {
  check_null_pointer(e);
  SEVENT_ONEQ *ex = event_oneq_from_event(e);
  JOB *packet = ex->packet;
  SQUEUE_TYPE *qt = packet->queue;

  if (e->type != EVENT_END_SERVICE)
    return ERR_EVENT_TYPE_FAIL;

  try ( oneq_update_time(state, e->time) );
  packet->etime = e->time;
  packet->state = JOB_STATE_OUT;
  qt->finish_packet(qt, packet);
  smeasurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( oneq_free_packet(state, packet) );

  while (qt->is_idle(qt) && (qt->get_waiting_length(qt) > 0))
    _process_packet(state, conf);

  return SUCCESS;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int _system_clean (SCHED_STATE_OPS* ops, SCHED_CONFIG *conf) {
  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  if (conf->queue_conf.out_file)
    fclose(conf->queue_conf.out_file);

  return SUCCESS;
}

/**
 * Process an event.
 * @param e : Event
 * @param conf : User configuration
 * @param ops :Abstract system operations
 * @return Error code (more in def.h and error.h)
 */
static int _process_event (SCHED_STATE_OPS *ops, SCHED_CONFIG *conf, SEVENT *e) {
  ONEQ_STATE *state = get_oneq_state_from_ops(ops);
  int (*func)(void*,void*,void*) = NULL;
  check_null_pointer(e);
  func = state->future_events.process[e->type];
  func(e, state, conf);
  return SUCCESS;
}

/**
 * Get next event from event list
 * @param ops : Abstract system operations
 * @param e : returned event
 * @return Error code (more in def.h and error.h)
 */
static int _get_next_event (SCHED_STATE_OPS *ops, SEVENT **e) {
  ONEQ_STATE *state = get_oneq_state_from_ops(ops);
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
  ONEQ_STATE *state = get_oneq_state_from_ops(ops);
  try (sevent_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

int oneq_set_ops (ONEQ_STATE *state) {
  check_null_pointer(state);
  state->ops.allow_continue = _allow_continue;
  state->ops.clean = _system_clean;
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

  sevent_list_init(&state->future_events, 2);
  sevent_list_reg_processing_func(&state->future_events, EVENT_ARRIVAL, _process_arrival);
  sevent_list_reg_processing_func(&state->future_events, EVENT_END_SERVICE, _process_end_service);

  smeasures_init (&state->measurement);
  squeue_man_init(&state->queues);

  sched_fifo_init(&fifo_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
  squeue_man_register_new_type(&state->queues, fifo_queue);
  state->queues.curr_queue = fifo_queue;
  //printf("Config to use queue %d \n", conf.queue_conf.type);
  squeue_man_activate_type(&state->queues, conf->queue_conf.type);

  oneq_set_ops(state);

  oneq_generate_event(state, EVENT_ARRIVAL, &conf->arrival_conf, NULL);

  return SUCCESS;
}
