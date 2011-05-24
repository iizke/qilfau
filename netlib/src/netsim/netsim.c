/**
 * @file netsim.c
 * Network simulator - the main framework.
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#include <string.h>
#include "netsim.h"
#include "conf/parser.h"
#include "../error.h"
#include "../queues/fifo.h"
//#include "sys_aqueue.h"
//#include "csma.h"

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

/**
 * Main program. Do parse user configuration file, and run a chosen simulation
 * @param nargs : number of parameters
 * @param args : parameters
 * @return Error code (see more in def.h and error.h)
 *
int start (int nargs, char** args) {
  SYS_STATE sys_state;
  CSMA_STATE csma_state;
  SYS_STATE_OPS *ops = NULL;
  int i = 0;

  /**
   * Initialize parameters of random, config, parse file configuration
   *

  random_init();
  /*
   * TEST PART
   *
  //test_gen_distribution();
  //check_chisqr_pvalue();
  //return 0;
  /************************************************
  if (nargs < 2) {
    printf("We need configuration \n");
    return -1;
  }

  config_init(&conf);
  try( parse_file (args[1]) );
  config_random_conf (&conf);

  /**
   * From user configuration, configure and initialize which module will
   * be executed (CSMA or one-queue system)
   *
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

  /// Start to do simulation
  pisas_sched(&conf, ops);

  /// Print out the measurement of simulation
  switch (conf.protocol) {
  case PROTOCOL_ONE_QUEUE:
    print_measurement(&sys_state.measurement);
    print_measurement(&((FIFO_QINFO*)(sys_state.queues.curr_queue->info))->measurement);
    break;
  case PROTOCOL_CSMA:
    print_measurement(&csma_state.measurement);
    for (i=0; i<csma_state.nqueues; i++)
      print_measurement(&((FIFO_QINFO*)(csma_state.queues[i].curr_queue->info))->measurement);
    break;
  }

  return SUCCESS;
}
*/
