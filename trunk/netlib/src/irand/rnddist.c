/**
 * @file rnddist.c
 * Random number in a well-known distribution.
 * Some methods are used: \n
 * (1) Inverse Transform method\n
 * (2) Convolution method \n
 * (3) Acceptance/Rejection technique (not implemented) \n
 * (4) Composition method \n
 * (5) Other (Normal, Poisson, Binomial) - not implemented \n
 *
 * @date Created on: May 3, 2011
 * @author iizke
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "irand.h"
#include "../error.h"

/**
 * Framework of generating random variable based on convolution method. \n
 * Y = X1 + X2 + ... + Xn \n
 * X1, X2,..., Xn are i.i.d random variable.
 * @param n : number of i.i.d random variables.
 * @param func : Generated random function of each random variable.
 * @param params : Parameters used for function func.
 * @return A random value.
 */
double _convolution_gen_rnd (int n, double (*func)(double, void*), void* params) {
  int i = 0;
  double sum = 0;
  for (i=0; i<n; i++) {
    double rndnum = irand_gen_random_real(RND_TYPE_LINEAR);
    sum += func(rndnum, params);
  }
  return sum;
}

/**
 * Framework of generating a random value (of distribution) based on
 * Inverse Transform method
 * @param inverse_func : inverse function (of a particular distribution).
 * @param params : parameters used for function inverse_func.
 * @return random value.
 */
double _inverse_gen_rnd (double (*inverse_func)(double, void*), void* params) {
  /// 1st step: Generate random integer number
  double rndnum = irand_gen_random_real(RND_TYPE_LINEAR);
  return inverse_func(rndnum, params);
}

/**
 * Inverse Transform function of Empirical Discrete distribution.
 * @param p : a uniform random number
 * @param params : parameters of this distribution includes: probabilities and
 * appropriate values. If list of value is not determined (NULL), then the return
 * value will be the index.
 * @return A real random number of this distribution
 */
double _inverse_empirical (double p, void *params) {
  struct empirical_params *eparams = params;
  int i = 0;
  double sum = 0;
  for (i = 0; i < eparams->n; i++) {
    if ((p <= (sum + eparams->probs[i])) && (p >= sum)) {
      if (!eparams->values)
        return i;
      return eparams->values[i];
    }
    sum += eparams->probs[i];
  }
  return 0;
}

/**
 * Framework of generating random variable based on composition method. \n
 * CDF(X) = p1*CDF1(X) + p1*CDF2(X) + ... + pn*CDFn(X) \n
 * @param n : Number of random variables.
 * @param rnd : List of random variables with specification of their distribution.
 * @param probs : List of probabilities for each random values.
 * @return A random value.
 */
double _composition_gen_rnd (int n, RANDOM_DIST *rnd, double *probs) {
  struct empirical_params eparams;
  int index = 0;
  eparams.n = n;
  eparams.probs = probs;
  eparams.values = 0; // null value
  index = (int)(_inverse_gen_rnd(_inverse_empirical, &eparams));
  return rnd->gen(rnd);
}


/**
 * Inverse Transform function of Exponential distribution
 * @param p : a uniform random number
 * @param params : structure that contains lambda (parameter of exponential distribution)
 * @return A real random number of exponential distribution
 */
double _inverse_exp (double p, void *params) {
  double *lambda = params;
  return (-log(1-p)/(*lambda));
}

/**
 * Inverse Transform function of Uniform distribution
 * @param p : a uniform random number
 * @param params : structure that contains parameter of uniform distribution
 * @return A real random number of uniform distribution
 */
double _inverse_uniform (double p, void* params) {
  struct uniform_params *range = params;
  return (range->from + (range->to - range->from)*p);
}

/**
 * Inverse Transform function of Discrete (integer) Uniform distribution
 * @param p : a uniform random number
 * @param params : structure that contains parameter of uniform distribution
 * @return A integer random number of uniform distribution
 */
double _inverse_int_uniform (double p, void* params) {
  struct uniform_params *range = params;
  return ceil(range->from + (range->to - range->from + 1)*p - 1);
}

/**
 * Inverse Transform function of Pareto distribution
 * @param p : a uniform random number
 * @param params : structure that contains lambda (parameter of pareto distribution)
 * @return A real random number of Pareto distribution
 */
double _inverse_pareto (double p, void *params) {
  double *lambda = params;
  return (1/pow(p, 1/(*lambda)));
}

/**
 * Inverse Transform function of Weibull distribution
 * @param p : a uniform random number
 * @param params : structure that contains parameter of Weibull distribution
 * @return A real random number of Weibull distribution
 */
double _inverse_weibull (double p, void *params) {
  struct weibull_params *_params = params;
  return (_params->a * pow(-log(p), 1/_params->b));
}

/**
 * Inverse Transform function of Bernoulli distribution
 * @param p : a uniform random number
 * @param params : pointer to a value that is a probability of success (1)
 * @return A real random number of Bernoulli distribution
 */
double _inverse_bernoulli (double p, void *params) {
  double *prob = params;
  return (p < (*prob))? 1 : 0;
}

/**
 * Inverse Transform function of Geometric distribution
 * @param p : a uniform random number
 * @param params : structure that contains parameter of Geometric distribution
 * @return A real random number of Geometric distribution
 */
double _inverse_geometric (double p, void *params) {
  double *prob = params;
  return  ceil((log(p)/log(1-(*prob)) - 1));
}

/**
 * Erlang Generator of random number
 * @param order : order of Erlang distribution
 * @param lambda : the average rate of Erlang variable
 * @return A real random number of Erlang-k distribution
 */
double irand_gen_erlang (int order, double lambda) {
  double lambda0 = lambda * order;
  return _convolution_gen_rnd(order, _inverse_exp, &lambda0);
}

/**
 * Exponential generator of random number
 * @param lambda : the average rate of random variable
 * @return A real random value
 */
double irand_gen_exp (double lambda) {
  return _inverse_gen_rnd(_inverse_exp, &lambda);
}

/**
 * Uniform generator of random number (real value)
 * @param from : lower-bound value
 * @param to : upper-bound value
 * @return A real random value in range [from, to]
 */
double irand_gen_uniform (double from, double to) {
  struct uniform_params up;
  up.from = from;
  up.to = to;
  return _inverse_gen_rnd(_inverse_uniform, &up);
}
/**
 * Integer Uniform generator of random number
 * @param from : lower-bound value
 * @param to : upper-bound value
 * @return A integer random value in range [from, to]
 */
double irand_gen_int_uniform (double from, double to) {
  struct uniform_params up;
  up.from = from;
  up.to = to;
  return _inverse_gen_rnd(_inverse_int_uniform, &up);
}

/**
 * Bernoulli generator of random number
 * @param prob : probability of success event (value 1)
 * @return A random value (0 or 1)
 */
double irand_gen_bernoulli (double prob) {
  return _inverse_gen_rnd(_inverse_bernoulli, &prob);
}

/**
 * Pareto generator of random number
 * @param lambda : parameter of Pareto distribution
 * @return A random number
 */
double irand_gen_pareto (double lambda) {
  return _inverse_gen_rnd(_inverse_pareto, &lambda);
}

int irand_mmpp_params_init (struct mmpp_params* p, FILE *f) {
  int i = 0 ;
  check_null_pointer(p);
  p->markov_state.type = MATRIX_TYPE_DENSE;
  matrix_setup_file(&p->markov_state, f);
  array_setup(&p->poisson_rate, sizeof(POINTER_VAL), p->markov_state.nrows);
  for (i = 0; i < p->markov_state.nrows; i++)
    /// No check wrong input here!
    fscanf(f, "%f", &p->poisson_rate.data[i].value);

  p->last_state = 0;
  p->last_time = 0;
  p->next_state = 0;
  p->next_time = 0;
  return SUCCESS;
}

/**
 * MMPP generator
 * @param p: parameter of MMPP distribution
 * @return
 */
double irand_gen_mmpp(struct mmpp_params * p) {
  float curr_rate = 0;
  float state_rate;
  float mint = 999999999;
  int i;

  while (curr_rate == 0) {
    curr_rate = array_get(&p->poisson_rate, p->last_state).value;
    p->last_time += irand_gen_exp(curr_rate);

    if (p->last_time >= p->next_time) {
      curr_rate = 0;
      p->last_time = p->next_time;
      p->last_state = p->next_state;
      // find next_state and next_time
      for (i=0; i<p->markov_state.ncols; i++) {
        // find min
        float t = 0;
        if (i == p->last_state) continue;
        state_rate = matrix_get_value(&p->markov_state, p->last_state, i).value;
        if (state_rate > 0) {
          t = irand_gen_exp(state_rate);
          if (mint > t) {
            mint = t;
            p->next_state = i;
          }
        }
      }
      p->next_time = p->last_time + mint;
      // regenerate random value
    }
  }
  return p->last_time;
}

int test_gen_distribution () {
  int i = 0;
  double avg = 0;
  for (i=0; i < 10000; i++) {
    avg += irand_gen_exp(10);
  }
  avg = avg / 10000;
  printf("avg of exponential (hope 0.1): %f \n", avg);

  avg = 0;
  for (i=0; i < 10000; i++) {
    avg += irand_gen_bernoulli(0.1);
  }
  avg = avg / 10000;
  printf("avg of bernoulli (hope 0.1): %f \n", avg);

  avg = 0;
  for (i=0; i < 10000; i++) {
    avg += irand_gen_uniform(10, 20);
  }
  avg = avg / 10000;
  printf("avg of uniform (hope 15): %f \n", avg);

  avg = 0;
  for (i=0; i < 10000; i++) {
    avg += irand_gen_erlang(2, 10);
  }
  avg = avg / 10000;
  printf("avg of erlang (hope 0.1): %f \n", avg);

  avg = 0;
  for (i=0; i < 10000; i++) {
    avg += irand_gen_pareto(10);
  }
  avg = avg / 10000;
  printf("avg of pareto: %f \n", avg);

  return 0;
}
