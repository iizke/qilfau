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
    sn->sdev = sqrtf((sn->tmpsumsqr - sn->tmpsum * sn->avg) / (sn->num_samples - 1));
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
  sn->sdev = sqrtf((sn->tmpsumsqr - sn->tmpsum * sn->avg) / (sn->all_time));
  sn->min = min(sn->min, sample);
  sn->max = max(sn->max, sample);
  sn->last_time = time;
  return SUCCESS;
}

double stat_num_calc_confidence_interval (STAT_NUM *sn, double confidence) {
  double c = get_normal_pvalue((confidence + 1)/2);
  return c*sn->sdev/sqrt(sn->num_samples);
}

int stat_num_merge(STAT_NUM *n1, STAT_NUM *n2) {
 n1->all_time += n2->all_time;
 n1->tmpsum += n2->tmpsum;
 n1->tmpsumsqr += n2->tmpsumsqr;
 n1->num_samples += n2->num_samples;
 if (n1->all_time <= 0) {
   n1->avg = n1->tmpsum / n1->num_samples;
   if (n1->num_samples > 1)
     n1->sdev = sqrtf((n1->tmpsumsqr - n1->tmpsum * n1->avg) / (n1->num_samples - 1));
 } else {
   n1->avg = n1->tmpsum / n1->all_time;
   n1->sdev = sqrtf((n1->tmpsumsqr - n1->tmpsum * n1->avg) / (n1->all_time));
 }

 n1->min = min(n1->min, n2->min);
 n1->max = max(n1->max, n2->max);
 return SUCCESS;
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
