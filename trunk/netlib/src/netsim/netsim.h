/**
 * @file netsim.h
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef NETSIM_H_
#define NETSIM_H_

#include "event.h"
#include "queues/measures.h"
#include "conf/config.h"

/// Functions of a simulated system
typedef struct system_state_operations SYS_STATE_OPS;
/// Functions of a simulated system
struct system_state_operations {
  /// Get next event from event list
  int (*get_next_event) (SYS_STATE_OPS*, EVENT **e);
  /// Remove an event out of event list
  int (*remove_event) (SYS_STATE_OPS *, EVENT *e);
  /// Check whether the program is stopped (from user configuration)
  int (*allow_continue) (CONFIG*, SYS_STATE_OPS*);
  /// Generate new event
  EVENT* (*generate_event) (int type, PACKET*, CONFIG*, SYS_STATE_OPS*);
  /// Process an event
  int (*process_event) (EVENT *e, CONFIG*, SYS_STATE_OPS*);
  /// Clean the simulated system (when finishing simulation)
  int (*clean) (CONFIG*, SYS_STATE_OPS*);
};

int netsim_start (char *conf_file);
int netsim_start_thread (char *conf_file);
int pisas_sched (void *conf, SYS_STATE_OPS *sys_ops);

#endif /* CONFIG_H_ */
