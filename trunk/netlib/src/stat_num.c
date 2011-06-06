/**
 * @file stat_num.c
 * Statistical calculation
 *
 * @date Created on: Apr 12, 2011
 * @author iizke
 */

#include <string.h>
#include <stdio.h>
#include "stat_num.h"
#include "error.h"
#include "math.h"
#include "quantile.h"

/// Calculate minimum of two values
#define min(a,b)  (a==0?b:(a<b?a:b))
/// Calculate maximum of two value
#define max(a,b)  (a<b?b:a)

/**
 * Calculate the statistical values of a random variable through its samples.
 * Used for discrete random variable.
 * @param sn : Statistical info
 * @param sample : new sample is collected to statistical info
 * @return Error code (defined in error.h)
 */
int stat_num_new_sample (STAT_NUM *sn, float sample) {
  check_null_pointer(sn);
  sn->tmpsum += sample;
  sn->tmpsumsqr += sample*sample;
  sn->num_samples++;
  //sn->avg = calc_avg(sn->avg, sn->num_samples, sample);
  sn->avg = sn->tmpsum / sn->num_samples;
  if (sn->num_samples > 1)
    sn->var = sqrtf((sn->tmpsumsqr - sn->tmpsum * sn->avg) / (sn->num_samples - 1));
  sn->min = min(sn->min, sample);
  sn->max = max(sn->max, sample);
  return SUCCESS;
}

/**
 * Calculate the statistical values of a random variable through its samples.
 * Used for continous random variable.
 * @param sn : statistical information
 * @param sample : new sample value
 * @param time : happened time for new sample
 * @return Error code (defined in error.h)
 */
int stat_num_new_time_sample (STAT_NUM *sn, float sample, float time) {
  float duration = time - sn->last_time;
  float t = sample * duration;
  check_null_pointer(sn);
  sn->tmpsum += t;
  sn->tmpsumsqr += t*sample;
  sn->num_samples++;
  sn->all_time += duration;
  sn->avg = sn->tmpsum / sn->all_time;
  sn->var = sqrtf((sn->tmpsumsqr - sn->tmpsum * sn->avg) / (sn->all_time));
  sn->min = min(sn->min, sample);
  sn->max = max(sn->max, sample);
  sn->last_time = time;
  return SUCCESS;
}

double stat_num_calc_confidence_interval (STAT_NUM *sn, double confidence) {
  double c = get_normal_pvalue(confidence);
  return c*sn->var/sqrt(sn->num_samples);
}

/**
 * Initialization of STAT_NUM structure (statistical information)
 * @param m : STAT_NUM
 * @return Error code (defined in error.h)
 */
int stat_num_init (STAT_NUM *m) {
  check_null_pointer(m);
  memset(m, 0, sizeof(STAT_NUM));
  return SUCCESS;
}