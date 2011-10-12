/**
 * @file netsim.h
 * @date Created on: Oct 7, 2011
 * @author iizke
 */

#ifndef SCHEDSIM_H_
#define SCHEDSIM_H_

#include "event.h"
#include "queues/measures.h"
#include "queues/job.h"
#include "conf/config.h"

/// Functions of a simulated system
typedef struct schedsim_state_operations SCHED_STATE_OPS;
/// Functions of a simulated system
struct schedsim_state_operations {
  /// Get next event from event list
  int (*get_next_event) (SCHED_STATE_OPS*, SEVENT **e);
  /// Remove an event out of event list
  int (*remove_event) (SCHED_STATE_OPS *, SEVENT *e);
  /// Check whether the program is stopped (from user configuration)
  int (*allow_continue) (SCHED_STATE_OPS*, SCHED_CONFIG*);
  /// Generate new event
  //SEVENT* (*generate_event) (int type, JOB*, SCHED_CONFIG*, SCHED_STATE_OPS*);
  /// Process an event
  int (*process_event) (SCHED_STATE_OPS*, SCHED_CONFIG*, SEVENT *e);
  /// Clean the simulated system (when finishing simulation)
  int (*clean) (SCHED_STATE_OPS*, SCHED_CONFIG*);
};

int schedsim_start (char *conf_file);
int schedsim_start_thread (char *conf_file);
int pisas_do_sched (void *conf, SCHED_STATE_OPS *sys_ops);
#endif /* CONFIG_H_ */
