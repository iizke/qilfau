/**
 * @file chisqr.c
 * Chi-Square test
 *
 * @date Created on: May 17, 2011
 * @author iizke
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../error.h"
#include "../random.h"
#include "../quantile.h"

typedef struct test_params {
  int nsamples;
  int nintervals;
  double p_value;
  RANDOM_DIST random_dist;
} TEST_PARAMS;

int test_chisqr (TEST_PARAMS *tp) {
  int i = 0;
  double x = 0;
  double delta = 0;
  double e = 0;
  double * samples = NULL;
  int * intervals = NULL;

  check_null_pointer(tp);
  samples = malloc_gc (sizeof(double) * tp->nsamples);
  check_null_pointer(samples);
  intervals = malloc_gc (sizeof(int) * tp->nintervals);
  check_null_pointer(intervals);
  memset(intervals, 0, sizeof(int)*tp->nintervals);
  /// Generate pseudo random samples and count them in intervals
  /// i/n <= sample < (i+1)/n ====> (sample*n - 1) < i <= sample*n
  for (i = 0; i < tp->nsamples; i++) {
    int level = 0;
    samples[i] = tp->random_dist.gen(tp->random_dist.params);
    level = floor(samples[i]);
    intervals[level]++;
  }
  /// Compute and compare chi-square value
  for (i = 0; i < tp->nintervals; i++) {
    e = tp->random_dist.cdf(&tp->random_dist, (i+1)/tp->nintervals);
    e -= tp->random_dist.cdf(&tp->random_dist, i/tp->nintervals);
    delta = (intervals[i] - e);
    x += delta*delta/e;
  }
  free_gc (intervals);
  free_gc (samples);
  if (x > get_chisqr_pvalue(tp->nintervals-1, tp->p_value))
    return FALSE;

  return TRUE;
}

int check_chisqr_pvalue () {
  printf("(9, 0.05) = (16.92) %f\n", get_chisqr_pvalue(1, 0.95));
  return SUCCESS;
}
