/**
 * @file queue_babs.c
 * Simulation of system with one queue, but burst arrival
 *
 * @date Created on: Aug 06, 2013
 * @author iizke
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "error.h"
#include "queues/burst_fifo.h"
#include "queue_babs.h"

extern BABSQ_CONFIG babs_conf;

/**
 * Create new packet
 * @param state : System state
 * @param p : new packet
 * @return Error code (see more in def.h and error.h)
 */
static int babs_new_packet (BABSQ_STATE *state, PACKET **p) {
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
static int babs_free_packet (BABSQ_STATE *state, PACKET *p) {
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
static int babs_update_time (EVENT *e, BABSQ_STATE *state) {
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
EVENT* babs_generate_arrival (BABSQ_CONFIG *conf, BABSQ_STATE *state) {
  EVENT *e = NULL;
  int error_code = SUCCESS;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_ARRIVAL;
  error_code = event_setup(e, &conf->arrival_conf, state->curr_time);
  if (error_code < 0) {
    free_gc (e);
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
EVENT* babs_generate_end_service (PACKET *p, BABSQ_CONFIG *conf, BABSQ_STATE *state) {
  EVENT *e = NULL;
  event_list_new_event(&state->future_events, &e);
  e->info.type = EVENT_END_SERVICE;
  event_setup(e, &conf->service_conf, state->curr_time);
  e->info.packet = p;
  event_list_insert_event(&state->future_events, e);
  //state->measurement.busy_time += (e->info.time.real - state->curr_time.real);
  return e;
}

/**
 * Check whether system should be stopped or not.
 * @param conf : User configuration
 * @param ops : Abstract system operators
 * @return 0 if system should be stopped, 1 otherwise.
 */
int babs_allow_continue (BABSQ_CONFIG *conf, SYS_STATE_OPS *ops) {
  BABSQ_STATE *state = get_babs_state_from_ops(ops);

  STOP_CONF *stop_conf = &conf->stop_conf;
  if ((stop_conf->max_time > 0 && state->curr_time.real > stop_conf->max_time) ||
      (stop_conf->max_arrival > 0 && state->measurement.total_arrivals > stop_conf->max_arrival) ||
      ((conf->arrival_conf.type == RANDOM_FILE) && (conf->arrival_conf.from_file) && (feof(conf->arrival_conf.from_file)))) {

    /** these commands stops the process of simulation
     *  waiting for processing all events
     */
    conf->arrival_conf.type = RANDOM_OTHER;
    conf->arrival_conf.distribution.gen = NULL;
    if (conf->stop_conf.queue_zero == STOP_QUEUE_ZERO) {
      QUEUE_TYPE *qt = NULL;
      qt = state->queues.curr_queue;
      if (qt->get_waiting_length(qt) > 0)
        return 1;
    } else {
      if (!event_list_is_empty(&state->future_events))
        return 1;
    }
    return 0;
  }
  return 1;
}

static int babs_setup_burst (BABSQ_STATE *state, RANDOM_CONF *burst_conf) {
  long burst = 0;
  double burstd = 0;
  //iprint(LEVEL_ERROR, "burst-trace conf: type %d, lambda %f\n", burst_conf->type, burst_conf->lambda);
  switch (burst_conf->type) {
  case RANDOM_FILE:
    //iprint(LEVEL_INFO, "Burst values from file are not supported, assign constant value = 1\n");
    burstd = 1;
    break;
  case RANDOM_MARKOVIAN:
    burstd = 1/(burst_conf->distribution.gen(&burst_conf->distribution));
    break;
  default:
    if (burst_conf->distribution.gen == NULL) {
      iprint(LEVEL_WARNING, "Burst type is not supported \n");
      burstd = 1;
    } else burstd = (burst_conf->distribution.gen(&burst_conf->distribution));
    break;
  }
  // tracing burst generation
  stat_num_new_sample(&state->burst_trace, burstd);

  if (burstd < 1) burst = 1;
  else burst = (long)burstd;
  //iprint(LEVEL_ERROR, "burst = %d | %f, type %d, lambda %f\n", burst, burstd, burst_conf->type, burst_conf->lambda);

  return burst;
}

/**
 * Process a packet
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
static int babs_process_packet (BABSQ_CONFIG *conf, BABSQ_STATE *state) {
  QUEUE_TYPE *qt = state->queues.curr_queue;
  EVENT *e = NULL;
  PACKET *p = NULL;

  qt->select_waiting_packet(qt, &p);
  p->info.stime.real = state->curr_time.real;
  qt->process_packet(qt, p);
  measurement_collect_data(&state->measurement, p, state->curr_time);
  e = babs_generate_end_service (p, conf, state);
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
int babs_packet_from_event (EVENT *e, PACKET *p) {
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
int babs_process_arrival (EVENT *e, BABSQ_CONFIG *conf, BABSQ_STATE *state) {
  PACKET *packet = NULL;
  QUEUE_TYPE *qt = state->queues.curr_queue;
  int burst = 0;

  try ( babs_update_time(e, state) );
  try ( babs_new_packet(state, &packet) );
  burst = babs_setup_burst(state, &(conf->burst_conf));
  if (burst > conf->queue_conf.num_servers) burst = conf->queue_conf.num_servers;
  packet->info.burst = burst;
  babs_packet_from_event (e, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  qt->push_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  if (packet->info.state == PACKET_STATE_DROPPED)
    babs_free_packet(state, packet);

  babs_generate_arrival(conf, state);

  while ((qt->is_servable(qt)) && (qt->get_waiting_length(qt) > 0))
    babs_process_packet(conf, state);

  return SUCCESS;
}

/**
 * Process end-service event
 * @param e : event
 * @param conf : user configuration
 * @param state : system state
 * @return Error code (see more in def.h and error.h)
 */
int babs_process_end_service (EVENT *e, BABSQ_CONFIG *conf, BABSQ_STATE *state) {
  PACKET *packet = e->info.packet;
  QUEUE_TYPE *qt = state->queues.curr_queue;

  try ( babs_update_time(e, state) );
  babs_packet_from_event(e, packet);
  qt->finish_packet(qt, packet);
  measurement_collect_data(&state->measurement, packet, state->curr_time);
  try ( babs_free_packet(state, packet) );

  while (qt->is_servable(qt) && (qt->get_waiting_length(qt) > 0))
    babs_process_packet(conf, state);

  return SUCCESS;
}

/**
 * Clean the system.
 * @param conf : user configuration
 * @param ops  : Abstract system operations
 * @return Error code (see more in def.h and error.h)
 */
static int babs_system_clean (BABSQ_CONFIG *conf, SYS_STATE_OPS *ops) {
  //ONEQ_STATE *state = get_sys_state_from_ops(ops);

  if (conf->arrival_conf.to_file)
    fclose(conf->arrival_conf.to_file);
  //if (state->departure_file)
  //fclose(state->departure_file);
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
EVENT * babs_generate_event(int type, PACKET *p, BABSQ_CONFIG *conf, SYS_STATE_OPS *ops) {
  BABSQ_STATE *state = get_babs_state_from_ops(ops);
  EVENT *e = NULL;

  switch (type) {
  case EVENT_ARRIVAL:
    e = babs_generate_arrival( conf, state);
    break;
  case EVENT_END_SERVICE:
    e = babs_generate_end_service(p, conf, state);
    break;
  default:
    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", type);
    break;
  }
  return e;
}

/**
 * Do save an event into file.
 * @param e : Event
 * @param conf : Configuration
 * @return Error code (see more in def.h and error.h)
 */
static int babs_save_event (EVENT *e, BABSQ_CONFIG *conf) {
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    event_save(e, conf->arrival_conf.to_file);
    break;
  case EVENT_END_SERVICE:
    event_save(e, conf->queue_conf.out_file);
    break;
  }
  return SUCCESS;
}

/**
 * Process an event.
 * @param e : Event
 * @param conf : User configuration
 * @param ops :Abstract system operations
 * @return Error code (more in def.h and error.h)
 */
int babs_process_event (EVENT *e, BABSQ_CONFIG *conf, SYS_STATE_OPS *ops) {
  BABSQ_STATE *state = get_babs_state_from_ops(ops);
  switch (e->info.type) {
  case EVENT_ARRIVAL:
    babs_process_arrival(e, conf, state);
    break;
  case EVENT_END_SERVICE:
    babs_process_end_service(e, conf, state);
    break;
  default:
    iprint(LEVEL_WARNING, "This kind of system does not support this event (%d)\n", e->info.type);
    break;
  }
  babs_save_event(e, conf);
  return SUCCESS;
}

/**
 * Get next event from event list
 * @param ops : Abstract system operations
 * @param e : returned event
 * @return Error code (more in def.h and error.h)
 */
int babs_get_next_event (SYS_STATE_OPS *ops, EVENT **e) {
  BABSQ_STATE *state = get_babs_state_from_ops(ops);
  event_list_get_first(&state->future_events, e);
  return SUCCESS;
}

/**
 * Remove an event out of event list
 * @param ops : Abstract system operations
 * @param e : Event
 * @return Error code (more in def.h and error.h)
 */
int babs_remove_event (SYS_STATE_OPS *ops, EVENT *e) {
  BABSQ_STATE *state = get_babs_state_from_ops(ops);
  try (event_list_remove_event(&state->future_events, e) );
  return SUCCESS;
}

/**
 * Initialize system state of one-queue system
 * @param state : system state
 * @param conf : user configuration
 * @return Error code (more in def.h and error.h)
 */
int babs_state_init (BABSQ_STATE *state, BABSQ_CONFIG *conf) {
  QUEUE_TYPE *burst_fifo_queue = NULL;
  check_null_pointer(state);

  state->curr_time.real = 0;
  packet_list_init(&state->free_packets, LL_CONF_STORE_FREE);
  event_list_init(&state->future_events);
  measures_init (&state->measurement);
  queue_man_init(&state->queues);
  stat_num_init(&state->burst_trace);

  burst_fifo_init(&burst_fifo_queue, conf->queue_conf.num_servers, conf->queue_conf.max_waiters);
  queue_man_register_new_type(&state->queues, burst_fifo_queue);
  state->queues.curr_queue = burst_fifo_queue;
  //printf("Config to use queue %d \n", conf.queue_conf.type);
  queue_man_activate_type(&state->queues, conf->queue_conf.type);

  state->ops.allow_continue = babs_allow_continue;
  state->ops.clean = babs_system_clean;
  state->ops.generate_event = babs_generate_event;
  state->ops.get_next_event = babs_get_next_event;
  state->ops.process_event = babs_process_event;
  state->ops.remove_event = babs_remove_event;
  return SUCCESS;
}

/**
 * Signal handler (SIGINT and SIGTERM) used whenever main process is stopped by
 * user (normally with Ctrl+C).
 * @param n : interrupt number (no use)
 */
static void babs_sig_handler(int n) {
  //netsim_print_result(&conf);
  //netsim_print_theorical_result(&conf);
  ((SYS_STATE_OPS*)babs_conf.runtime_state)->clean(&babs_conf, babs_conf.runtime_state);
  exit(1);
}

/**
 * Main program. Do parse user configuration file, and run a chosen simulation
 * @param nargs : number of parameters
 * @param args : parameters
 * @return Error code (see more in def.h and error.h)
 */
int babs_start (char *conf_file) {
  BABSQ_STATE babs_state;
  SYS_STATE_OPS *ops = NULL;

  signal(SIGINT, babs_sig_handler);
  signal(SIGTERM, babs_sig_handler);

  random_init();
  try( config_parse_babs_file (conf_file) );
  //return SUCCESS;
  switch (babs_conf.protocol) {
  case PROTOCOL_BABSQ:
    babs_state_init (&babs_state, &babs_conf);
    ops = &babs_state.ops;
    break;
  default:
    iprint(LEVEL_WARNING, "This protocol is not supported right now \n");
    return SUCCESS;
  }
  babs_conf.runtime_state = ops;
  pisas_sched(&babs_conf, ops);

  if (babs_state.curr_time.real > babs_state.measurement.last_idle_time)
    babs_state.measurement.busy_time += babs_state.curr_time.real - babs_state.measurement.last_idle_time;

  print_measurement(&babs_state.measurement);
  print_statistical_value("Burst-trace", &babs_state.burst_trace, 0.999);
  return SUCCESS;
}

