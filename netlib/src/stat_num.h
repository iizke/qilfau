/**
 * @file stat_num.h
 * Statistical number structure
 *
 * @date Created on: Apr 12, 2011
 * @author iizke
 */

#ifndef STAT_NUM_H_
#define STAT_NUM_H_

/**
 * Statistical information of a random process
 */
typedef struct statistical_number {
  /// Minimum value
  float min;
  /// Maximum value
  float max;
  /// Average value (mean)
  float avg;
  /// Variance value
  float var;
  /// Time of observing this random process
  float all_time;
  /// Temporary value calculating sum of value
  float tmpsum;
  /// Temporary value calculating sum of value square
  float tmpsumsqr;
  /// The last time of observation
  float last_time;
  /// Number of samples in observation
  int num_samples;
} STAT_NUM;

int stat_num_new_sample (STAT_NUM *snum, float sample);
int stat_num_new_time_sample (STAT_NUM *sn, float sample, float time);
int stat_num_init (STAT_NUM *snum);
double stat_num_calc_confidence_interval (STAT_NUM *sn, double confidence);
int stat_num_merge(STAT_NUM *n1, STAT_NUM *n2);
#endif /* STAT_NUM_H_ */
