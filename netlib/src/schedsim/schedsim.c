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
#include <pthread.h>
#include <math.h>
#include <time.h>
#include "error.h"
#include "math_util.h"
#include "queues/fifo.h"
#include "conf/parser.h"
#include "conf/config.h"
#include "sys_aqueue.h"
#include "schedsim.h"

extern SCHED_CONFIG conf;
extern long debug;
//long debug = LEVEL_EWI; // print all Info message

/**
 * Do save an event into file.
 * @param e : Event
 * @param conf : Configuration
 * @return Error code (see more in def.h and error.h)
 */
static int _save_event (SEVENT *e, SCHED_CONFIG *conf) {
  switch (e->type) {
  case EVENT_ARRIVAL:
    sevent_save(e, conf->arrival_conf.to_file);
    break;
  case EVENT_END_SERVICE:
    sevent_save(e, conf->queue_conf.out_file);
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
int pisas_do_sched (void *conf, SCHED_STATE_OPS *sys_ops) {
  check_null_pointer(conf);
  check_null_pointer(sys_ops);

  while (sys_ops->allow_continue(sys_ops, conf)) {
    // assume that event_list support sorting follow time
    SEVENT *e = NULL;
    sys_ops->get_next_event(sys_ops, &e);

//    if (!e) {
//      // There is no event, we need one
//      sys_ops->generate_event(EVENT_ARRIVAL, NULL, conf, sys_ops);
//      //sys_ops->generate_event(EVENT_ARRIVAL, NULL, conf, sys_ops);
//      continue;
//    }

    iprint (LEVEL_INFO, "Get event type %d at %f \n", e->type, e->time);
    sys_ops->process_event(sys_ops, conf, e);
    _save_event(e,conf);
    //sevent_list_remove_event(&future_events, e);
    sys_ops->remove_event(sys_ops, e);
  }
  sys_ops->clean(sys_ops, conf);
  return SUCCESS;
}

/**
 * Print out result of measurement of simulation
 * @param conf : User configuration
 */
static void schedsim_print_result(SCHED_CONFIG *conf) {
  ONEQ_STATE *sys_state;
  switch (conf->protocol) {
    case PROTOCOL_ONE_QUEUE:
      sys_state = get_oneq_state_from_ops(((SCHED_STATE_OPS*)conf->runtime_state));
      print_smeasurement(&sys_state->measurement);
      //print_smeasurement(&((QUEUE_FF*)(sys_state->queues.curr_queue->info))->measurement);
      break;
    default:
      break;
  }
}

#define is_M_M_1(_atype,_stype,_nservers,_qlen) \
  ((_atype == RANDOM_MARKOVIAN) && (_stype == RANDOM_MARKOVIAN) && \
      (_nservers == 1) && (_qlen < 0))

#define is_M_M_Inf(_atype,_stype,_nservers,_qlen) \
  ((_atype == RANDOM_MARKOVIAN) && (_stype == RANDOM_MARKOVIAN) && \
      (_nservers < 0))

#define is_M_M_C(_atype,_stype,_nservers,_qlen) \
  ((_atype == RANDOM_MARKOVIAN) && (_stype == RANDOM_MARKOVIAN) && \
      (_nservers > 1) && (_qlen < 0))

#define is_M_M_1_K(_atype,_stype,_nservers,_qlen) \
  ((_atype == RANDOM_MARKOVIAN) && (_stype == RANDOM_MARKOVIAN) && \
      (_nservers == 1) && (_qlen > 0))

#define is_M_M_C_K(_atype,_stype,_nservers,_qlen) \
  ((_atype == RANDOM_MARKOVIAN) && (_stype == RANDOM_MARKOVIAN) && \
      (_nservers > 0) && (_qlen > 0))

static void schedsim_print_theorical_mm1 (double arrival, double service) {
  double ro = arrival/service;
  // Number of customers in system (in queue + in service)
  double slen = ro/(1-ro);
  double var_slen = ro/((1-ro)*(1-ro));
  double qlen = ro*slen;
  double var_qlen = slen*(slen + ro*ro);
  printf("In theory, M/M/1 has: \n");
  printf("%20s : mean %4.5f, var %4.5f\n", "#customers in sys", slen, sqrtf(var_slen));
  printf("%20s : mean %4.5f, var %4.5f\n", "#customers in queue", qlen, sqrtf(var_qlen));
  printf("%20s : mean %4.5f\n", "System response time", slen/arrival );
  printf("%20s : mean %4.5f\n", "Queue response time", qlen/arrival );
}

/**
 * Theorical result of M/M/C queueing system
 * @param ar : arrival rate
 * @param sr : service rate
 * @param c : number of servers
 */
static void schedsim_print_theorical_mmc (double ar, double sr, int c) {
  double p0 = 0;
  double ro = ar/(sr*c);
  double qlen = 0;
  int i = 0;
  for (i=0; i<c; i++)
    p0 += pow(ro*c, i)/math_factorial(i);

  p0 = 1/(p0 + pow(ro*c,c)/(math_factorial(c)*(1-ro)));
  qlen = p0 * pow(ar/sr, c)*ar*sr / (math_factorial(c-1)*pow(c*sr-ar,2));

  printf("In theory, M/M/C has: \n");
  printf("%20s : mean %4.5f\n", "#customers in sys", qlen + ar/sr);
  printf("%20s : mean %4.5f\n", "#customers in queue", qlen);
  printf("%20s : mean %4.5f\n", "System response time", qlen/ar + 1/sr);
  printf("%20s : mean %4.5f\n", "Queue response time", qlen/ar);
}

static void schedsim_print_theorical_mm (double ar, double sr) {
  printf("In theory, M/M/Inf has: \n");
  printf("%20s : mean %4.5f\n", "#customers in sys", ar/sr);
  printf("%20s : mean 0\n", "#customers in queue");
  printf("%20s : mean %4.5f\n", "System response time", 1/sr);
  printf("%20s : mean 0\n", "Queue response time");
}

static void schedsim_print_theorical_mm1k (double ar, double sr, int maxlen) {
  double p0 = 0;
  double pk = 0;
  double ro = ar/sr;
  double slen = 0;
  double qlen = 0;
  if (ro != 1) {
    p0 = (1-ro)/(1-pow(ro,maxlen+1));
    slen = ro*(1 - (maxlen+1)*pow(ro, maxlen) +
                maxlen*pow(ro, maxlen+1)) / ((1-ro)*(1-pow(ro,maxlen+1)));
  } else {
    p0 = 1/(maxlen+1);
    slen = maxlen/2;
  }
  pk = p0*pow(ro,maxlen);

  qlen = slen + p0 - 1;
  printf("In theory, M/M/1/K has: \n");
  printf("%20s : mean %4.5f\n", "#customers in sys", slen);
  printf("%20s : mean %4.5f\n", "#customers in queue", qlen);
  printf("%20s : mean %4.5f\n", "System response time", slen/(ar*(1-pk)));
  printf("%20s : mean %4.5f\n", "Queue response time", qlen/(ar*(1-pk)));
  printf("%20s : mean %4.5f\n", "Throughput", ar*(1-pk));
  printf("%20s : mean %4.5f\n", "Utilization", (1-p0));
}

static void schedsim_print_theorical_result (SCHED_CONFIG *conf) {
  int atype = conf->arrival_conf.type;
  int stype = conf->service_conf.type;
  int nserver = conf->queue_conf.num_servers;
  int qlen = conf->queue_conf.max_waiters;
  if (is_M_M_1(atype, stype, nserver, qlen))
    schedsim_print_theorical_mm1(conf->arrival_conf.lambda, conf->service_conf.lambda);
  if (is_M_M_C(atype, stype, nserver, qlen))
    schedsim_print_theorical_mmc(conf->arrival_conf.lambda, conf->service_conf.lambda, nserver);
  if (is_M_M_Inf(atype, stype, nserver, qlen))
    schedsim_print_theorical_mm(conf->arrival_conf.lambda, conf->service_conf.lambda);
  if (is_M_M_1_K(atype, stype, nserver, qlen))
    schedsim_print_theorical_mm1k(conf->arrival_conf.lambda, conf->service_conf.lambda, qlen);
}

/**
 * Signal handler (SIGINT and SIGTERM) used whenever main process is stopped by
 * user (normally with Ctrl+C).
 * @param n : interrupt number (no use)
 */
static void netsim_sig_handler(int n) {
  schedsim_print_result(&conf);
  schedsim_print_theorical_result(&conf);
  ((SCHED_STATE_OPS*)conf.runtime_state)->clean(conf.runtime_state, &conf);
  exit(1);
}

static void* netsim_thread (SCHED_CONFIG *conf) {
  ONEQ_STATE* sys_state;
  SCHED_MEASURES *m = NULL;
  SCHED_CONFIG local_conf;
  SCHED_STATE_OPS *ops = NULL;
  time_t start = time(NULL);

  memcpy(&local_conf, conf, sizeof(SCHED_CONFIG));
  local_conf.stop_conf.max_time /= local_conf.nthreads;
  local_conf.stop_conf.max_arrival /= local_conf.nthreads;
  switch (local_conf.protocol) {
  case PROTOCOL_ONE_QUEUE:
    sys_state = malloc_gc(sizeof(ONEQ_STATE));
    sched_sys_state_init (sys_state, &local_conf);
    ops = &sys_state->ops;
    m = &sys_state->measurement;
    break;
  default:
    iprint(LEVEL_WARNING, "This protocol is not supported right now \n");
    return NULL;
  }
  local_conf.runtime_state = ops;
  pisas_do_sched(&local_conf, ops);
  printf("Thread info: \n");
  schedsim_print_result(&local_conf);
  printf("Thread run time: %d (seconds) \n", time(NULL) - start);
  //netsim_print_theorical_result(&conf);
  return m;
}

/**
 * Main program. Do parse user configuration file, and run a chosen simulation
 * @param nargs : number of parameters
 * @param args : parameters
 * @return Error code (see more in def.h and error.h)
 */
int schedsim_start (char *conf_file) {
  ONEQ_STATE sys_state;
  SCHED_STATE_OPS *ops = NULL;

  signal(SIGINT, netsim_sig_handler);
  signal(SIGTERM, netsim_sig_handler);

  random_init();
  try( schedconf_parse_file (conf_file) );

  switch (conf.protocol) {
  case PROTOCOL_ONE_QUEUE:
    sched_sys_state_init (&sys_state, &conf);
    ops = &sys_state.ops;
    break;
  default:
    iprint(LEVEL_WARNING, "This protocol is not supported right now \n");
    return SUCCESS;
  }
  conf.runtime_state = ops;
  pisas_do_sched(&conf, ops);

  schedsim_print_result(&conf);
  schedsim_print_theorical_result(&conf);
  return SUCCESS;
}

/**
 * Start network simulation with multiple threads.
 * The simulation is parallel based on time partition method.
 * @param conf_file : Configuration file
 * @return Error code (see def.h and error.h)
 */
int schedsim_start_thread (char *conf_file) {
  pthread_t *threads;
  SCHED_MEASURES m;
  SCHED_MEASURES *lm; // local measurement
  int i;
  time_t start;

  start = time(NULL);
  try( schedconf_parse_file (conf_file) );
  threads = malloc_gc(sizeof(pthread_t)*conf.nthreads);
  check_null_pointer(threads);

  signal(SIGINT, netsim_sig_handler);
  signal(SIGTERM, netsim_sig_handler);

  random_init();
  for (i = 0; i < conf.nthreads; i++)
    pthread_create(&threads[i], NULL, netsim_thread, &conf);

  smeasures_init(&m);
  for (i = 0; i < conf.nthreads; i++) {
    pthread_join(threads[i], &lm);
    smeasurement_merge(&m, lm);
  }
  printf("MERGING RESULT:\n");
  print_smeasurement(&m);
  //netsim_print_result(&conf);
  schedsim_print_theorical_result(&conf);
  printf("Time of simulation: %d (seconds) \n", time(NULL)-start);
  return SUCCESS;
}
