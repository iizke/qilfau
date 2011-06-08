/**
 * @file measures.c
 * Measurement functions
 *
 * @date Created on: Apr 9, 2011
 * @author iizke
 */
#include <string.h>
#include <stdio.h>
#include "error.h"
#include "packet.h"
#include "measures.h"
#include "queue_man.h"

/**
 * Initialization of structure MEASURES
 * @param m : pointer to Measures structure
 * @return Error code (defined in def.h)
 */
int measures_init (MEASURES *m) {
  check_null_pointer(m);
  memset(m, 0, sizeof(MEASURES));
  return SUCCESS;
}

#define print_statistical_value(_var_name,_var, _conf) { \
    printf("%20s : mean %4.5f, var %4.5f, min %4.3f, max %4.3f, confidency %4.3f\n", \
      _var_name, \
      (_var)->avg, \
      (_var)->var, \
      (_var)->min, \
      (_var)->max, \
      stat_num_calc_confidence_interval(_var, _conf)); }

/**
 * Print out to screen measured information
 * @param m : pointer to a MEASURES structure.
 * @return Error code (defined in def.h)
 */
int print_measurement (MEASURES *m) {
  check_null_pointer(m);
  print_statistical_value("#customers in queue", &m->queue_len, 0.9);
  print_statistical_value("Queue response time", &m->waittime, 0.9);
  print_statistical_value("service time", &m->servtime, 0.9);
  print_statistical_value("inter-arrival time", &m->interarrival_time, 0.9);
  printf("%20s : %d \n", "total arrival",(int)m->total_arrivals);
  printf("%20s : %d \n","total departure", (int)m->total_departures);
  printf("%20s : %d \n", "total dropped", (int)m->total_dropped);
  printf("%20s : %.3f \n", "total time", m->total_time);
  printf("%20s : %4.5f \n","throughput", m->total_departures/m->total_time);
  printf("%20s : %4.5f \n","utilization", (m->total_departures * m->servtime.avg / m->total_time));
  printf("%20s : %4.5f \n","drop probability", (float)m->total_dropped/(float)m->total_arrivals);

  return SUCCESS;
}

/**
 * Collect new data into measurement structure
 * @param m : measurement
 * @param p : packet
 * @param curr_time : Current time
 * @return Error code (see more in def.h and error.h)
 */
int measurement_collect_data (MEASURES *m, PACKET *p, TIME curr_time) {
  QUEUE_TYPE *qt = NULL;
  check_null_pointer(p);
  check_null_pointer(m);
  qt = p->info.queue;

  switch (p->info.state) {
  case PACKET_STATE_PROCESSING:
    stat_num_new_sample(&m->waittime, p->info.stime.real - p->info.atime.real);
    stat_num_new_time_sample(&m->queue_len, qt->get_waiting_length(qt) + 1, curr_time.real);
    break;
  case PACKET_STATE_WAITING:
    stat_num_new_time_sample(&m->queue_len, qt->get_waiting_length(qt) - 1, curr_time.real);
    break;
  case PACKET_STATE_DROPPED:
    m->total_dropped++;
    break;
  case PACKET_STATE_IN:
    m->total_arrivals++;
    stat_num_new_sample(&m->interarrival_time, curr_time.real - m->last_arrival_time);
    m->last_arrival_time = curr_time.real;
    break;
  case PACKET_STATE_OUT:
    m->total_departures++;
    stat_num_new_sample(&m->servtime, p->info.etime.real - p->info.stime.real);
    //stat_num_new_time_sample(&m->queue_len, qt->get_waiting_length(qt) + 1, curr_time.real);
    //stat_num_new_sample(&m->waittime, curr_time.real - p->info.atime.real);
    break;
  default:
    iprint(LEVEL_WARNING, "Packet state is not support\n");
    return ERR_PACKET_STATE_WRONG;
    break;
  }
  m->total_time = curr_time.real;
  return SUCCESS;
}

int measurement_merge (MEASURES *m1, MEASURES *m2) {
  check_null_pointer(m1);
  check_null_pointer(m2);
  stat_num_merge(&m1->interarrival_time, &m2->interarrival_time);
  stat_num_merge(&m1->queue_len, &m2->queue_len);
  stat_num_merge(&m1->servtime, &m2->servtime);
  stat_num_merge(&m1->waittime, &m2->waittime);
  m1->total_arrivals += m2->total_arrivals;
  m1->total_departures += m2->total_departures;
  m1->total_dropped += m2->total_dropped;
  m1->total_time += m2->total_time;
  return SUCCESS;
}
