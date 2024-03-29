/**
 * @file measures.h
 * Collect statistical information of queueing system
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef SCHED_MEASURES_H_
#define SCHED_MEASURES_H_

#include "def.h"
#include "stat_num.h"
#include "job.h"

//#include "packet.h"

/**
 * MEASURE structure used to store some results when simulating queue system
 */
typedef struct sched_measures {
  /// Total number of arrival events appearing in simulation
  long total_arrivals;
  /// Total number of departure packets/events at output in simulation
  long total_departures;
  /// Total number of dropped packets
  long total_dropped;
  /// Total time of simulation
  float total_time;
  /// Statistical value of queue length
  STAT_NUM queue_len;
  /// Statistical value of service time
  STAT_NUM servtime;
  /// Statistical value of waiting time
  STAT_NUM waittime;
  /// Statistical value of inter-arrival time
  STAT_NUM interarrival_time;
  /// Last value of arrival time (temporary variable supporting to compute interarrival_time
  float last_arrival_time;
} SCHED_MEASURES;

#define print_statistical_value(_var_name,_var, _conf) { \
    printf("%20s : mean %4.5f, sdev %4.5f, min %4.3f, max %4.3f, confidency %4.3f\n", \
      _var_name, \
      (_var)->avg, \
      (_var)->sdev, \
      (_var)->min, \
      (_var)->max, \
      stat_num_calc_confidence_interval(_var, _conf)); }

int smeasures_init (SCHED_MEASURES *m);
int print_smeasurement (SCHED_MEASURES *m);
int smeasurement_collect_data (SCHED_MEASURES *m, JOB *p, double curr_time);
int smeasurement_merge (SCHED_MEASURES *m1, SCHED_MEASURES *m2);
#endif /* MEASURES_H_ */
