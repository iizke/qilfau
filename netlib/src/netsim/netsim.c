/**
 * @file netsim.c
 * Network simulator - the main framework.
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "../error.h"
#include "../queues/fifo.h"
#include "conf/parser.h"
#include "conf/config.h"
#include "sys_aqueue.h"
#include "csma.h"
#include "netsim.h"

extern CONFIG conf;
extern long debug;
//long debug = LEVEL_EWI; // print all Info message

/**
 * Setup time of event based on its statistical characteristics.
 * @param e : Event
 * @param fc : random configuration
 * @param curr_time : Current time
 * @return Error code (see more in def.h and error.h)
 */
int event_setup (EVENT *e, RANDOM_CONF *fc, TIME curr_time) {
  float time;
  int type;
  float etime;
  switch (fc->type) {
  case RANDOM_MARKOVIAN:
    e->info.time.real = curr_time.real + gen_exponential(fc->lambda);
    break;
  case RANDOM_FILE:
    fscanf(fc->from_file, "%f %d %f", &time, &type, &etime);
    e->info.time.real = time;
    iprintf(LEVEL_INFO, "Get time from file, value %f \n", e->info.time.real);
    break;
  case RANDOM_UNIFORM:
    e->info.time.real = curr_time.real + gen_uniform(fc->from, fc->to);
    break;
  default:
    iprintf(LEVEL_WARNING, "Flow type is not supported \n");
    return ERR_RANDOM_TYPE_FAIL;
    break;
  }
  return SUCCESS;
}

/**
 * Do save an event into file.
 * @param e : Event
 * @param conf : Configuration
 * @return Error code (see more in def.h and error.h)
 */
static int _save_event (EVENT *e, CONFIG *conf) {
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
 * Scheduling events while simulating.
 * @param conf : Configuration from user, including: arrival distribution,
 * execution distribution, waiting line, ...
 * @param sys_ops : Operations of System state.
 * @return Error code (defined in libs/error.h and def.h)
 */
int pisas_sched (CONFIG *conf, SYS_STATE_OPS *sys_ops) {
  check_null_pointer(conf);
  check_null_pointer(sys_ops);

  while (sys_ops->allow_continue(conf, sys_ops)) {
    // assume that event_list support sorting follow time
    EVENT *e = NULL;
    sys_ops->get_next_event(sys_ops, &e);

    if (!e) {
      // There is no event, we need one
      sys_ops->generate_event(EVENT_ARRIVAL, NULL, conf, sys_ops);
      continue;
    }

    iprintf (LEVEL_INFO, "Get event type %d at %f \n", e->info.type, e->info.time.real);
    sys_ops->process_event(e, conf, sys_ops);
    _save_event(e,conf);
    //event_list_remove_event(&future_events, e);
    sys_ops->remove_event(sys_ops, e);
  }
  sys_ops->clean(conf, sys_ops);
  return SUCCESS;
}

static void netsim_print_result(CONFIG *conf) {
  SYS_STATE *sys_state;
  CSMA_STATE *csma_state;
  int i = 0;
  switch (conf->protocol) {
    case PROTOCOL_ONE_QUEUE:
      sys_state = get_sys_state_from_ops(((SYS_STATE_OPS*)conf->runtime_state));
      print_measurement(&sys_state->measurement);
      print_measurement(&((FIFO_QINFO*)(sys_state->queues.curr_queue->info))->measurement);
      break;
    case PROTOCOL_CSMA:
      csma_state = get_csma_state_from_ops(((SYS_STATE_OPS*)conf->runtime_state));
      print_measurement(&csma_state->measurement);
      for (i=0; i<csma_state->nqueues; i++)
        print_measurement(&((FIFO_QINFO*)(csma_state->queues[i].curr_queue->info))->measurement);
      break;
    default:
      break;
  }
}

static void netsim_sig_handler(int n, siginfo_t *info, void *data) {
  netsim_print_result(&conf);
  printf("Cleaning system ...\n");
  ((SYS_STATE_OPS*)conf.runtime_state)->clean(&conf, conf.runtime_state);
  printf("Stopping program ... Done\n");
  exit(1);
}

/**
 * Main program. Do parse user configuration file, and run a chosen simulation
 * @param nargs : number of parameters
 * @param args : parameters
 * @return Error code (see more in def.h and error.h)
 */
int netsim_start (char *conf_file) {
  SYS_STATE sys_state;
  CSMA_STATE csma_state;
  SYS_STATE_OPS *ops = NULL;
  struct sigaction ac;
  ac.sa_sigaction = netsim_sig_handler;
  ac.sa_flags = SA_SIGINFO;
  sigaction(SIGINT, &ac, NULL);
  sigaction(SIGTERM, &ac, NULL);

  random_init();
  try( config_parse_file (conf_file) );

  switch (conf.protocol) {
  case PROTOCOL_CSMA:
    csma_state_init (&csma_state, &conf);
    ops = &csma_state.ops;
    break;
  case PROTOCOL_ONE_QUEUE:
    sys_state_init (&sys_state, &conf);
    ops = &sys_state.ops;
    break;
  default:
    iprintf(LEVEL_WARNING, "This protocol is not supported right now \n");
    return SUCCESS;
  }
  conf.runtime_state = ops;
  pisas_sched(&conf, ops);
  netsim_print_result(&conf);
  return SUCCESS;
}
