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
#include "irand/irand.h"
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
  long seed = irand_get_seed();
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return (float)irand_gen_uniform((double)from, (double)to);
  case LIB_RANDOM_POLIRAND:
    return polirand_uniform(from, to, &seed);
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
  long seed = irand_get_seed();
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return irand_gen_int_uniform(from, to);
  case LIB_RANDOM_POLIRAND:
    return polirand_randint(from, to, &seed);
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
  long seed = irand_get_seed();
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return (float)irand_gen_exp((double)lambda);
  case LIB_RANDOM_POLIRAND:
    return polirand_negexp(1/lambda, &seed);
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
  /// seed used in polirand-functions
  long seed = irand_get_seed();
  switch (librand) {
  case LIB_RANDOM_POLIRAND:
    return polirand_poisson(lambda, &seed);
  case LIB_RANDOM_IRAND:
  case LIB_RANDOM_RANDLIB:
  default:
    return ignpoi(lambda);
  }
  return 0;
}

/**
 * Generate a real number following Bernoulli distribution
 * @param p : Probability of success event (return 1)
 * @return 0 or 1
 */
int gen_bernoulli(double p) {
  float _p;
  switch (librand) {
  case LIB_RANDOM_IRAND:
    return irand_gen_bernoulli(p);
  case LIB_RANDOM_POLIRAND:
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
  up = malloc_gc (sizeof(struct uniform_params));
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
  l = malloc_gc (sizeof(double));
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
  l = malloc_gc (sizeof(double));
  *l = prob;
  rd->params = l;
  return SUCCESS;
}

double random_dist_gen_const (RANDOM_DIST *rd) {
  double* val = NULL;
  check_null_pointer(rd);
  val = rd->params;
  return (*val);
}

int random_dist_init_const (RANDOM_DIST *rd, double *val) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_const;
  //rd->cdf = random_dist_cdf_const;
  rd->params = val;
  return SUCCESS;
}

double random_dist_gen_mmpp (RANDOM_DIST *rd) {
  struct mmpp_params *p = NULL;
  check_null_pointer(rd);
  p = rd->params;
  return irand_gen_mmpp(p);
}

int random_dist_init_mmpp(RANDOM_DIST *rd, struct mmpp_params *p) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_mmpp;
  rd->params = p;
  return SUCCESS;
}

double random_dist_gen_mmpp_r (RANDOM_DIST *rd) {
  struct mmpp_r_params *p = NULL;
  check_null_pointer(rd);
  p = rd->params;
  return irand_gen_mmpp_r(p);
}

int random_dist_init_mmpp_r(RANDOM_DIST *rd, struct mmpp_r_params *p) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_mmpp_r;
  rd->params = p;
  return SUCCESS;
}

/**
 * Generator of poisson distribution in an RANDOM_DIST
 * @param rd : random distribution structure
 * @return A real number followed the exponential distribution
 */
double random_dist_gen_poisson (RANDOM_DIST *rd) {
  double *lambda = 0;
  long ret = 0;
  check_null_pointer(rd);
  lambda = rd->params;
  ret = gen_poisson((float)(*lambda));
  return (double)ret;
}

/**
 * Initialize random distribution structure of poisson distribution
 * @param rd : Random distribution structure
 * @param lambda : pointer of lambda value
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_poisson(RANDOM_DIST *rd, double *lambda) {
  check_null_pointer(rd);
  rd->gen = random_dist_gen_poisson;
  rd->cdf = NULL;
  rd->params = lambda;
  return SUCCESS;
}

struct normal_param {
  float mean;
  float sdev;
};

/**
 * Generator of normal distribution in an RANDOM_DIST
 * @param rd : random distribution structure
 * @return A real number followed the exponential distribution
 */
double random_dist_gen_normal (RANDOM_DIST *rd) {
  struct normal_param *param;
  double ret = 0;
  check_null_pointer(rd);
  param = rd->params;
  ret = (double)gen_normal(param->mean, param->sdev);
  return ret;
}

/**
 * Initialize random distribution structure of normal distribution
 * @param rd : Random distribution structure
 * @param mean : mean value
 * @param sdev : standard deviation value
 * @return Error code (see more in def.h and error.h)
 */
int random_dist_init_normal(RANDOM_DIST *rd, float mean, float sdev) {
  struct normal_param * param = NULL;
  check_null_pointer(rd);
  rd->gen = random_dist_gen_normal;
  rd->cdf = NULL;
  param = gc_malloc(sizeof(struct normal_param));
  param->mean = mean;
  param->sdev = sdev;
  rd->params = param;
  return SUCCESS;
}

int random_dist_init_normal_empty(RANDOM_DIST *rd) {
  struct normal_param * param = NULL;
  check_null_pointer(rd);
  rd->gen = random_dist_gen_normal;
  rd->cdf = NULL;
  param = gc_malloc(sizeof(struct normal_param));
  rd->params = param;
  return SUCCESS;
}

int random_dist_normal_set_mean (RANDOM_DIST *rd, float mean) {
  struct normal_param *param = NULL;
  check_null_pointer(rd);
  check_null_pointer(rd->params);
  param = (struct normal_param*)rd->params;
  param->mean = mean;
  return SUCCESS;
}

float random_dist_normal_get_mean (RANDOM_DIST *rd) {
  struct normal_param *param = NULL;
  check_null_pointer(rd);
  check_null_pointer(rd->params);
  param = (struct normal_param*)rd->params;
  return param->mean;
}

float random_dist_normal_get_sdev (RANDOM_DIST *rd) {
  struct normal_param *param = NULL;
  check_null_pointer(rd);
  check_null_pointer(rd->params);
  param = (struct normal_param*)rd->params;
  return param->sdev;
}

int random_dist_normal_set_sdev (RANDOM_DIST *rd, float sdev) {
  struct normal_param *param = NULL;
  check_null_pointer(rd);
  check_null_pointer(rd->params);
  param = (struct normal_param*)rd->params;
  param->sdev = sdev;
  return SUCCESS;
}

float random_dist_get_mean (RANDOM_DIST *rd) {
  float fval;
  if (rd->gen == random_dist_gen_const || rd->gen == random_dist_gen_bernoulli || rd->gen == random_dist_gen_poisson){
    double * val = (double*)(rd->params);
    fval = (float)(*val);
    return fval;
  }
  if (rd->gen == random_dist_gen_exp) {
    double * val = (double*)(rd->params);
    fval = (float)(1/ (*val));
    return fval;
  }
  if (rd->gen == random_dist_gen_normal)
    return random_dist_normal_get_mean(rd);
  if (rd->gen == random_dist_gen_uniform){
    struct uniform_params *up = rd->params;
    fval = (float)((up->from + up->to)/2);
    return fval;
  }
  return 0;
}

float random_dist_get_sdev (RANDOM_DIST *rd) {
  if (rd->gen == random_dist_gen_const)
    return 0;

  if (rd->gen == random_dist_gen_bernoulli){
    double pr = (double)(*(double*)(rd->params));
    return (float)(sqrt(pr*(1-pr)));
  }

  if (rd->gen == random_dist_gen_poisson){
    double l = (double)(*(double*)(rd->params));
    return (float)(sqrt(l));
  }

  if (rd->gen == random_dist_gen_exp)
    return (float)(1/ (*(double*)(rd->params)));

  if (rd->gen == random_dist_gen_normal)
    return random_dist_normal_get_sdev(rd);

  if (rd->gen == random_dist_gen_uniform){
    struct uniform_params *up = rd->params;
    return (float)((up->to - up->from)/sqrt(12));
  }
  return 0;
}


