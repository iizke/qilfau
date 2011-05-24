/**
 * @file random.c
 * Random distribution and generator (interfaces)
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "random.h"
#include "error.h"
//#include "conf/config.h"

int librand = LIB_RANDOM_IRAND;

/**
 * Initialize some parameters of random generators
 * @return SUCCESS
 */
int random_init () {
  irand_init();
  irand_random_seed();
  return SUCCESS;
}

/**
 * Generate a real number following uniform distribution
 * @param from,to : range of generated value
 * @return a real number in range
 */
float gen_uniform(float from, float to) {
  //extern CONFIG conf;
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return (float)irand_gen_uniform((double)from, (double)to);
  case LIB_RANDOM_RANDLIB:
  default:
    return genunf(from, to);
  }
}

/**
 * Generate a integer number following uniform distribution
 * @param from,to : range of generated value
 * @return an integer number in range
 */
long gen_int_uniform(long from, long to) {
  //extern CONFIG conf;
  //switch (conf.random_lib) {
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return irand_gen_int_uniform(from, to);
  case LIB_RANDOM_RANDLIB:
  default:
    return ignuin(from, to);
  }
}

/**
 * Generate a real number following exponential distribution
 * @param lambda : lambda
 * @return a real number
 */
float gen_exponential(float lambda){
  // rate ~ lambda
  //extern CONFIG conf;
  //switch (conf.random_lib) {
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return (float)irand_gen_exp((double)lambda);
  case LIB_RANDOM_RANDLIB:
  default:
    return genexp(1/lambda);
  }
}

/**
 * Generate a real number following normal distribution
 * @param mean : mean
 * @param sd : standard deviation
 * @return a real number
 */
float gen_normal(float mean, float sd) {
  return gennor(mean, sd);
}

/**
 * Generate a real number following Poisson distribution
 * @param lambda : lambda
 * @return an integer number
 */
long gen_poisson(float lambda){
  return ignpoi(lambda);
}

/**
 * Generate a real number following Bernoulli distribution
 * @param p : Probability of success event (return 1)
 * @return 0 or 1
 */
int gen_bernoulli(double p) {
  //extern CONFIG conf;
  float _p;
  //switch (conf.random_lib) {
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return irand_gen_bernoulli(p);
  case LIB_RANDOM_RANDLIB:
  default:
    _p = (float)p;
    return (int)ignbin(1, _p);
  }

}

/**
 * Generator of uniform distribution in an RANDOM_DIST
 * @param rd : random distribution structure
 * @return A real number followed the uniform distribution
 */
double random_dist_gen_uniform (RANDOM_DIST *rd) {
  struct uniform_params *up = rd->params;
  return gen_uniform(up->from, up->to);
}

/**
 * Cumulative distribution function of uniform distribution
 * @param rd : Random distribution structure
 * @param x : A real value
 * @return CDF value
 */
double random_dist_cdf_uniform (RANDOM_DIST *rd, double x) {
  struct uniform_params *up = ((struct uniform_params*)(rd->params));
  if (x < up->from)
    return 0;
  if (x >= up->to)
    return 1;
  return (x - up->from)/(up->to - up->from);
}

/**
 * Initialize random distribution structure of uniform distribution
 * @param rd : Random distribution structure
 * @param from : Lower bound value
 * @param to : Upper bound value
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_uniform1 (RANDOM_DIST *rd, double from, double to) {
  struct uniform_params *up = NULL;
  check_null_pointer(rd);
  up = malloc (sizeof(struct uniform_params));
  check_null_pointer(up);
  up->from = from;
  up->to = to;
  rd->params = up;
  rd->gen = random_dist_gen_uniform;
  rd->cdf = random_dist_cdf_uniform;
  return SUCCESS;
}

/**
 * Initialize random distribution structure of uniform distribution
 * @param rd : Random distribution structure
 * @param up : structure containing upper bound and lower bound of uniform distribution
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_uniform0 (RANDOM_DIST *rd, struct uniform_params *up) {
  check_null_pointer(rd);
  check_null_pointer(up);
  rd->params = up;
  rd->gen = random_dist_gen_uniform;
  rd->cdf = random_dist_cdf_uniform;
  return SUCCESS;
}


/**
 * Generator of exponential distribution in an RANDOM_DIST
 * @param rd : random distribution structure
 * @return A real number followed the exponential distribution
 */
double random_dist_gen_exp (RANDOM_DIST *rd) {
  double *lambda = 0;
  check_null_pointer(rd);
  lambda = rd->params;
  return gen_exponential(*lambda);
}

/**
 * Cumulative distribution function of exponential distribution
 * @param rd : Random distribution structure
 * @param x : A real value
 * @return CDF value
 */
double random_dist_cdf_exp (RANDOM_DIST *rd, double x) {
  check_null_pointer(rd);
  double *lambda = (rd->params);
  return (1 - exp(-x*(*lambda)));
}

/**
 * Initialize random distribution structure of exponential distribution
 * @param rd : Random distribution structure
 * @param lambda : pointer of lambda value
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_exp0(RANDOM_DIST *rd, double *lambda) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_exp;
  rd->cdf = random_dist_cdf_exp;
  rd->params = lambda;
  return SUCCESS;
}

/**
 * Initialize random distribution structure of exponential distribution
 * @param rd : Random distribution structure
 * @param lambda : lambda value
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_exp1(RANDOM_DIST *rd, double lambda) {
  double *l = 0;
  check_null_pointer(rd);
  rd->gen = random_dist_gen_exp;
  rd->cdf = random_dist_cdf_exp;
  l = malloc (sizeof(double));
  *l = lambda;
  rd->params = l;
  return SUCCESS;
}

/**
 * Generate random value extracted from file.
 * @param rd : random distribution structure
 * @return A real psuedo random number (from a file)
 */
double random_dist_gen_file (RANDOM_DIST *rd) {
  FILE *f = (FILE*)(rd->params);
  float value;
  if (!f)
    return 0;

  fscanf(f, "%f", &value);
  return (double)value;
}

/**
 * Initialize random structure extracted from file
 * @param rd : Random distribution structure
 * @param f : pointer to a file
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_file0(RANDOM_DIST *rd, FILE *f) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_file;
  rd->cdf = NULL;
  rd->params = f;
  return SUCCESS;
}

/**
 * Generator of random value of bernoulli distribution
 * @param rd : random distribution structure
 * @return A real number followed the bernoulli distribution
 */
double random_dist_gen_bernoulli (RANDOM_DIST *rd) {
  double *prob = 0;
  check_null_pointer(rd);
  prob = rd->params;
  return gen_bernoulli(*prob);
}

/**
 * Cumulative distribution function of bernoulli distribution
 * @param rd : Random distribution structure
 * @param x : A real value
 * @return CDF value
 */
double random_dist_cdf_bernoulli (RANDOM_DIST *rd, double x) {
  double *p = rd->params;
  if (x < 0)
    return 0;
  if ((x >=0) && (x < 1))
    return ( 1 - (*p));
  else
    return 1;
}

/**
 * Initialize random distribution structure of bernoulli distribution
 * @param rd : Random distribution structure
 * @param prob : pointer of probability of success value (value 1)
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_bernoulli0 (RANDOM_DIST *rd, double * prob) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_bernoulli;
  rd->cdf = random_dist_cdf_bernoulli;
  rd->params = prob;
  return SUCCESS;
}

/**
 * Initialize random distribution structure of bernoulli distribution
 * @param rd : Random distribution structure
 * @param prob : probability of success value (value 1)
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_bernoulli1(RANDOM_DIST *rd, double prob) {
  double *l = 0;
  check_null_pointer(rd);
  rd->gen = random_dist_gen_bernoulli;
  rd->cdf = random_dist_cdf_bernoulli;
  l = malloc (sizeof(double));
  *l = prob;
  rd->params = l;
  return SUCCESS;
}
