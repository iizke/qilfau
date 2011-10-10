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

static SEVENT* _generate_sleep(ONEQ_STATE *state, SCHED_CONFIG *conf, JOB *job) {
  SEVENT *e = NULL;
  int error_code = SUCCESS;
  sevent_list_new_event(&state->future_events, &e);
  e->type = EVENT_SLEEP;
  error_code = sevent_setup(e, &conf->service_conf, state->curr_time);
  if (error_code < 0) {
    free_gc (e);
    return NULL;
  }
  sevent_list_insert_event(&state->future_events, e);
  return e;
}

/**
 * Generate new event.
 * @param type : type of event
 * @param p : Packet related to this new event
 * @param conf : user configuration
 * @param ops : Abstract system operations
 * @return New event
 */
static SEVENT * rr_generate_event(int type, JOB *p, SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
  ONEQ_STATE *state = get_sys_state_from_ops(ops);
  SEVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    e = oneq_generate_arrival(state, conf);
    break;
  case EVENT_END_SERVICE:
    e = oneq_generate_end_service(state, p, conf);
    break;
  case EVENT_SLEEP:
    e = _generate_sleep (state, conf, p);
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
//static int rr_process_event (SEVENT *e, SCHED_CONFIG *conf, SCHED_STATE_OPS *ops) {
//  ONEQ_STATE *state = get_sys_state_from_ops(ops);
//  switch (e->type) {
//  case EVENT_ARRIVAL:
//    _process_arrival(e, conf, state);
//    break;
//  case EVENT_END_SERVICE:
//    _process_end_service(e, conf, state);
//    break;
//  default:
//    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", e->type);
//    break;
//  }
//  return SUCCESS;
//}

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
  sevent_list_init(&state->future_events);
  smeasures_init (&state->measurement);
  squeue_man_init(&state->queues);

  sched_rr_init(&rr_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters, conf->queue_conf.quantum);
  squeue_man_register_new_type(&state->queues, rr_queue);
  state->queues.curr_queue = rr_queue;
  //printf("Config to use queue %d \n", conf.queue_conf.type);
  squeue_man_activate_type(&state->queues, conf->queue_conf.type);

  oneq_set_ops(state);
  state->ops.generate_event = rr_generate_event;
  //state->ops.process_event = rr_process_event;

  return SUCCESS;
}
