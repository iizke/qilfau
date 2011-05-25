/**
 * @file measures.c
 * Measurement functions
 *
 * @date Created on: Apr 9, 2011
 * @author iizke
 */
#include <string.h>
#include <stdio.h>
#include "../error.h"
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

/**
 * Print out to screen measured information
 * @param m : pointer to a MEASURES structure.
 * @return Error code (defined in def.h)
 */
int print_measurement (MEASURES *m) {
  check_null_pointer(m);
  printf("queue length          : mean %4.2f, var %4.2f, min %4.2f, max %4.2f\n",
      m->queue_len.avg,
      m->queue_len.var,
      m->queue_len.min,
      m->queue_len.max);
  printf("waiting time          : mean %4.2f, var %4.2f, min %4.2f, max %4.2f\n",
      m->waittime.avg,
      m->waittime.var,
      m->waittime.min,
      m->waittime.max);
  printf("service time          : mean %4.2f, var %4.2f, min %4.2f, max %4.2f\n",
      m->servtime.avg,
      m->servtime.var,
      m->servtime.min,
      m->servtime.max);
  printf("inter-arrival time    : mean %4.2f, var %4.2f, min %4.2f, max %4.2f\n",
      m->interarrival_time.avg,
      m->interarrival_time.var,
      m->interarrival_time.min,
      m->interarrival_time.max);
  printf("total arrival         : %d \n", (int)m->total_arrivals);
  printf("total departure       : %d \n", (int)m->total_departures);
  printf("total dropped         : %d \n", (int)m->total_dropped);
  printf("total time            : %.3f \n", m->total_time);

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
    break;
  case PACKET_STATE_WAITING:
    stat_num_new_time_sample(&m->queue_len, qt->get_waiting_length(qt), p->info.atime.real);
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
    stat_num_new_time_sample(&m->queue_len, qt->get_waiting_length(qt), p->info.atime.real);
    break;
  default:
    iprintf(LEVEL_WARNING, "Packet state is not support\n");
    return ERR_PACKET_STATE_WRONG;
    break;
  }
  m->total_time = curr_time.real;
  return SUCCESS;
}
