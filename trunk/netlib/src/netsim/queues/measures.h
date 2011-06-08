/**
 * @file measures.h
 * Collect statistical information of queueing system
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef MEASURES_H_
#define MEASURES_H_

#include "def.h"
#include "stat_num.h"

/**
 * MEASURE structure used to store some results when simulating queue system
 */
typedef struct measures {
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
} MEASURES;

int measures_init (MEASURES *m);
int print_measurement (MEASURES *m);
int measurement_collect_data (MEASURES *m, PACKET *p, TIME curr_time);
int measurement_merge (MEASURES *m1, MEASURES *m2);
#endif /* MEASURES_H_ */
