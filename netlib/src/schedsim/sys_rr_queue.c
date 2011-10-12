/**
 * @file sys_rr_queue.c
 *
 * @date Created on: Oct 9, 2011
 * @author iizke
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "queues/rr.h"
#include "sys_aqueue.h"
#include "event_rr.h"

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
  QUEUE_RR * rrq = queue_type_get_rr_queue(qt);

  qt->select_waiting_packet(qt, &p);
  qt->process_packet(qt, p);

  if (p->remaining_time == 0) {
    e = oneq_generate_event(state, EVENT_END_SERVICE, &conf->service_conf, p);
    p->remaining_time = e->time - state->curr_time;
    p->stime = state->curr_time;
  }

  smeasurement_collect_data(&state->measurement, p, state->curr_time);

  if (p->remaining_time > conf->queue_conf.quantum) {
    if (e) state->ops.remove_event(&state->ops, e);
    e = oneq_generate_event(state, EVENT_SLEEP, NULL, p);
    e->time = state->curr_time + conf->queue_conf.quantum;
    sevent_list_insert_event(&state->future_events, e);
  } else if (!e) {
    e = oneq_generate_event(state, EVENT_END_SERVICE, NULL, p);
    e->time = state->curr_time + p->remaining_time;
    sevent_list_insert_event(&state->future_events, e);
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

  while ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) > 0))
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

static int _process_sleep (SEVENT *e, ONEQ_STATE *state, SCHED_CONFIG *conf) {
  check_null_pointer(e);
  SEVENT_ONEQ *ex = event_oneq_from_event(e);
  JOB *packet = ex->packet;
  SQUEUE_TYPE *qt = packet->queue;

  if (e->type != EVENT_SLEEP)
    return ERR_EVENT_TYPE_FAIL;

  try ( oneq_update_time(state, e->time) );
  packet->remaining_time -= conf->queue_conf.quantum;
  packet->state = JOB_STATE_WAITING;
  qt->finish_packet(qt, packet);
  if ((qt->is_idle(qt)) && (qt->get_waiting_length(qt) >= 1))
    _process_packet(state, conf);

  qt->push_packet(qt, packet);
  if (packet->state == JOB_STATE_DROPPED)
    oneq_free_packet(state, packet);

  return SUCCESS;
}

/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int sched_rrq_state_init (ONEQ_STATE *state, SCHED_CONFIG *conf) {
  SQUEUE_TYPE *rr_queue = NULL;
  sched_sys_state_init(state, conf);

  check_null_pointer(state);

  state->curr_time = 0;
  job_list_init(&state->free_packets, LL_CONF_STORE_FREE);

  sevent_list_init(&state->future_events, 3);
  sevent_list_reg_processing_func(&state->future_events, EVENT_ARRIVAL, _process_arrival);
  sevent_list_reg_processing_func(&state->future_events, EVENT_END_SERVICE, _process_end_service);
  sevent_list_reg_processing_func(&state->future_events, EVENT_SLEEP, _process_sleep);

  smeasures_init (&state->measurement);
  squeue_man_init(&state->queues);

  sched_rr_init(&rr_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters, conf->queue_conf.quantum);
  squeue_man_register_new_type(&state->queues, rr_queue);
  state->queues.curr_queue = rr_queue;
  squeue_man_activate_type(&state->queues, conf->queue_conf.type);

  oneq_set_ops(state);

  return SUCCESS;
}
