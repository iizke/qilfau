/**
 * @file irand.h
 * My implementation of Random number and distribution (called irand)
 *
 * @date Created on: Apr 28, 2011
 * @author iizke
 */

#ifndef IRAND_H_
#define IRAND_H_

#include "matrix/matrix.h"
#include "list/array.h"

/// Random generator: Linear Congruential
#define RND_TYPE_LINEAR               1
/// Random generator: Combined Linear Congruential
#define RND_TYPE_COMBINED             2
/// Random generator: Tausworthe (Not supported right now)
#define RND_TYPE_TAUSWORTHE           3

/// Parameters of Weibull distribution
struct weibull_params{
  /// Value of a
  double a;
  /// Value of b
  double b;
};

/// Parameters of Uniform distribution
struct uniform_params {
  /// Lower-bound value
  double from;
  /// Upper-bound value
  double to;
};

/// Parameters of Empirical Discrete distribution
struct empirical_params {
  /// Number of possible values
  int n;
  /// List of possible values
  double *values;
  /// List of probabilities for each values
  double *probs;
};

/// Parameters of MMPP (Markov Modulation Poisson Process) distribution
struct mmpp_params {
  MATRIX markov_state;
  ARRAY poisson_rate;
  int last_state;
  double last_time;
  int next_state;
  double next_time;
};

/// Random distribution structure (a framework)
typedef struct random_distribution RANDOM_DIST;
/// Random distribution structure (a framework)
struct random_distribution {
  /// Random number generator
  double (*gen) (RANDOM_DIST*);
  /// Cumulative Distributed Function
  double (*cdf) (RANDOM_DIST*, double);
  /// Parameter of distribution
  void *params;
};

int irand_init ();
int irand_new_seed (unsigned long seed);
int irand_random_seed();
unsigned long irand_gen_random (int type);
unsigned long irand_gen_srandom (int type, unsigned long seed);
double irand_gen_random_real (int type);
double irand_gen_srandom_real (int type, unsigned long seed);
unsigned long irand_gen_range_random (int type, unsigned long from, unsigned long to);

double irand_gen_erlang (int order, double lambda);
double irand_gen_exp (double lambda);
double irand_gen_uniform (double from, double to);
double irand_gen_int_uniform (double from, double to);
double irand_gen_bernoulli (double prob);
double irand_gen_pareto (double lambda);
#endif /* IRAND_H_ */