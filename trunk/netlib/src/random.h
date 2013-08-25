/**
 * @file random.h
 * Random generator and distribution structure
 *
 * @date Created on: Apr 6, 2011
 * @author iizke
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdio.h>
#include "irand/irand.h"
#include "ranlib/ranlib.h"
#include "polirand/random.h"

/// Random library implemented by iizke
#define LIB_RANDOM_IRAND                  0
/// Random library implemented by randlib
#define LIB_RANDOM_RANDLIB                1
/// Random library implemented by polito-er
#define LIB_RANDOM_POLIRAND               2

#define random_dist_gen(rdist) ((rdist)->gen(rdist))
#define random_dist_cdf(rdist, value) (rdist->cdf(rdist, value))

int random_init ();

float gen_uniform(float from, float to);
long gen_int_uniform(long from, long to);
//long gen_int_uniform_from (int libtype, long from, long to);
int gen_bernoulli(double p);
float gen_exponential(float rate); // rate ~ lambda

float gen_normal(float mean, float sd);

long gen_poisson(float lambda);

int random_dist_init_uniform1 (RANDOM_DIST *rd, double from, double to);
int random_dist_init_uniform0 (RANDOM_DIST *rd, struct uniform_params *up);
int random_dist_init_exp0(RANDOM_DIST *rd, double *lambda);
int random_dist_init_exp1(RANDOM_DIST *rd, double lambda) ;
int random_dist_init_file0(RANDOM_DIST *rd, FILE *f);
int random_dist_init_bernoulli0(RANDOM_DIST *rd, double* prob);
int random_dist_init_bernoulli1(RANDOM_DIST *rd, double prob);
int random_dist_init_const (RANDOM_DIST *rd, double *val);
int random_dist_init_mmpp(RANDOM_DIST *rd, struct mmpp_params *p);
int random_dist_init_mmpp_r(RANDOM_DIST *rd, struct mmpp_r_params *p);
int random_dist_init_poisson(RANDOM_DIST *rd, double *lambda);
int random_dist_init_normal(RANDOM_DIST *rd, float mean, float sdev);
int random_dist_init_normal_empty(RANDOM_DIST *rd);
int random_dist_normal_set_mean (RANDOM_DIST *rd, float mean);
int random_dist_normal_set_sdev (RANDOM_DIST *rd, float sdev);
float random_dist_normal_get_mean (RANDOM_DIST *rd);
float random_dist_normal_get_sdev (RANDOM_DIST *rd);
float random_dist_get_mean (RANDOM_DIST *rd);
float random_dist_get_sdev (RANDOM_DIST *rd);

#endif /* RANDOM_H_ */
