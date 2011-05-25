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

#define MAX_PVALUE_ID   11

double chisqr_pvalues[][MAX_PVALUE_ID] =
{
///p_value
/// 0.95    0.9   0.8   0.7   0.5   0.3    0.2    0.1     0.05    0.01   0.001
    {0.004,  0.02, 0.06, 0.15, 0.46, 1.07,  1.64,  2.71,   3.84,   6.64,  10.83},
    {0.100,  0.21, 0.45, 0.71, 1.39, 2.41,  3.22,  4.60,   5.99,   9.21,  13.82},
    {0.35,   0.58, 1.01, 1.42, 2.37, 3.66,  4.64,  6.25,   7.82,   11.34, 16.27},
    {0.71,   1.06, 1.65, 2.20, 3.36, 4.88,  5.99,  7.78,   9.49,   13.28, 18.47},
    {1.14,   1.61, 2.34, 3.00, 4.35, 6.06,  7.29,  9.24,   11.07,  15.09, 20.52},
    {1.63,   2.20, 3.07, 3.83, 5.35, 7.23,  8.56,  10.64,  12.59,  16.81, 22.46},
    {2.17,   2.83, 3.82, 4.67, 6.35, 8.38,  9.80,  12.02,  14.07,  18.48, 24.32},
    {2.73,   3.49, 4.59, 5.53, 7.34, 9.52,  11.03, 13.36,  15.51,  20.09, 26.12},
    {3.32,   4.17, 5.38, 6.39, 8.34, 10.66, 12.24, 14.68,  16.92,  21.67, 27.88},
    {3.94,   4.86, 6.18, 7.27, 9.34, 11.78, 13.44, 15.99,  18.31,  23.21, 29.59}
};

#define pvalue_id(p_value)   \
  ((p_value == 0.95)? 0 : \
  ((p_value == 0.9)? 1 : \
  ((p_value == 0.8)? 2 : \
  ((p_value == 0.7)? 3 : \
  ((p_value == 0.5)? 4 : \
  ((p_value == 0.3)? 5 : \
  ((p_value == 0.2)? 6 : \
  ((p_value == 0.1)? 7 : \
  ((p_value == 0.05)? 8 : \
  ((p_value == 0.01)? 9 : \
  ((p_value == 0.001)? 10 : (-1)) \
  ))))))))))

#define get_chisqr_pvalue(_d, _p)  (chisqr_pvalues[_d - 1][pvalue_id(_p)])

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
  samples = malloc (sizeof(double) * tp->nsamples);
  check_null_pointer(samples);
  intervals = malloc (sizeof(int) * tp->nintervals);
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
  free (intervals);
  free (samples);
  if (x > get_chisqr_pvalue(tp->nintervals-1, tp->p_value))
    return FALSE;

  return TRUE;
}

int check_chisqr_pvalue () {
  printf("(9, 0.05) = (16.92) %f\n", get_chisqr_pvalue(1, 0.95));
  return SUCCESS;
}
