/**
 * @file rndnum.c
 * Pseudo-Random number generators.
 * Some methods are used: \n
 * (1) Linear Congruential \n
 * (2) Combined Linear Congruential \n
 * (3) TAUSWORTHE (not implemented) \n
 *
 * @date Created on: Apr 20, 2011
 * @author iizke
 */

#include <stdio.h>
#include <time.h>
#include "../error.h"
#include "irand.h"

/**
 * Linear Congruential Generator
 */
typedef struct linear_congruential_gen {
  /// Module
  unsigned long m;
  /// Seed or X0
  unsigned long seed;
  /// Multiplier
  unsigned long mul;
  /// Increment
  unsigned long inc;
  /// The last pseudo-random value
  unsigned long last_value;
} LINEAR_LEHMER_GEN;

/**
 * Random number generator based on Combined-Linear-Congruential Generator
 * with two Linear Congruential Generator
 */
typedef struct combined_linear_congruential_gen {
  /// The first Linear Congruential Generator
  LINEAR_LEHMER_GEN gen1;
  /// The second Linear Congruential Generator
  LINEAR_LEHMER_GEN gen2;
  unsigned long last_value;
} COMBINED_LINEAR_GEN;

/// Pseudo Random generator using Linear Congruential technique.
LINEAR_LEHMER_GEN linear_rnd_gen;
/// Pseudo Random generator using Combined Linear Congruential technique.
COMBINED_LINEAR_GEN combined_rnd_gen;

/**
 * Linear Congruential Generator
 * @param gen (generator)
 * @return Random number
 */
unsigned long _linear_lehmer_gen_num (LINEAR_LEHMER_GEN *gen) {
  long q = 0;
  long r = 0;
  long g = 0;
  long h = 0;
  check_null_pointer(gen);
  q = gen->m / gen->mul;
  r = gen->m % gen->mul;
  if (gen->last_value <= 0)
    gen->last_value = gen->seed;
  g = (gen->last_value % q)*gen->mul - r*(gen->last_value / q);
  if (g < 0)
    h = 1;
  gen->last_value = (g + gen->m*h + gen->inc) % gen->m;
  return gen->last_value;
}

/**
 * Linear Congruential Generator
 * @param gen (generator)
 * @return Random number
 */
double _linear_lehmer_gen_real (LINEAR_LEHMER_GEN *gen) {
  check_null_pointer(gen);
  _linear_lehmer_gen_num(gen);
  return ((double)gen->last_value / (double)gen->m);
}

/**
 * Initialize the Linear Congruential Generator
 * @param gen : Linear Congruential Generator
 * @return Error code (defined in libs/error.h)
 */
int _linear_lehmer_gen_init (LINEAR_LEHMER_GEN *gen) {
  check_null_pointer(gen);
  /// Module = 2^31 - 1 = 2147483647
  gen->m = 2147483647;
  /// Multiplier = 7^5 = 16807
  gen->mul = 16807;
  /// Increment = 0
  gen->inc = 0;
  gen->seed = 1;
  return SUCCESS;
}

/**
 * Generate a pseudo random number based on Combined Linear Congruential Generator
 * @param gen : Combined Linear Congruential Generator
 * @return Random number
 */
unsigned long _combined_linear_gen_num (COMBINED_LINEAR_GEN *gen) {
  _linear_lehmer_gen_num(&gen->gen1);
  _linear_lehmer_gen_num(&gen->gen2);
  gen->last_value = (gen->gen1.last_value - gen->gen2.last_value) % (gen->gen1.m - 1);
  return gen->last_value;
}

/**
 * generate real number in range [0,1]
 * @param gen : Combined Linear Generator
 * @return real number
 */
double _combined_linear_gen_real (COMBINED_LINEAR_GEN *gen) {
  _combined_linear_gen_num(gen);
  if (gen->last_value == 0)
    return ((double)(gen->gen1.m - 1)/(double)gen->gen1.m);
  return ((double)gen->last_value / (double)gen->gen1.m);
}

/**
 * Initialize Combined Linear Congruential Generator
 * @param gen : Combined Linear Congruential Generator structure
 * @return Error code (defined libs/error.h)
 */
int _combined_linear_gen_init (COMBINED_LINEAR_GEN *gen) {
  check_null_pointer(gen);
  gen->gen1.mul = 40014;
  gen->gen1.inc = 0;
  gen->gen1.m = 2147483562;
  gen->gen1.seed = 1;
  gen->gen2.mul = 40692;
  gen->gen2.inc = 0;
  gen->gen2.m = 2147483399;
  gen->gen2.seed = 1;
  return SUCCESS;
}

/**
 * Pseudo Integer Random Uniform generator
 * @param type : type of algorithm (linear congruential or combined linear congruential)
 * @return Random value
 */
unsigned long irand_gen_random (int type) {
  return irand_gen_srandom(type, 0);
}

/**
 * Pseudo Real-value Random Uniform generator
 * @param type : type of algorithm (linear congruential or combined linear congruential)
 * @return Random value
 */
double irand_gen_random_real (int type) {
  return irand_gen_srandom_real(type, 0);
}

unsigned long irand_gen_srandom (int type, unsigned long seed) {
  extern LINEAR_LEHMER_GEN linear_rnd_gen;
  extern COMBINED_LINEAR_GEN combined_rnd_gen;

  switch (type) {
  case RND_TYPE_LINEAR:
    linear_rnd_gen.seed = seed>0?seed:linear_rnd_gen.seed;
    return _linear_lehmer_gen_num(&linear_rnd_gen);
    break;
  case RND_TYPE_COMBINED:
    /// Generate random number in Combined Linear Congruential
    combined_rnd_gen.gen1.seed = seed>0?seed:combined_rnd_gen.gen1.seed;
    return _combined_linear_gen_num(&combined_rnd_gen);
    break;
  case RND_TYPE_TAUSWORTHE:
  default:
    iprintf(LEVEL_WARNING, "Not support this type (%d) to generate random number\n", type);
    return 0;
  }
  return 0;
}

double irand_gen_srandom_real (int type, unsigned long seed) {
  extern LINEAR_LEHMER_GEN linear_rnd_gen;
  extern COMBINED_LINEAR_GEN combined_rnd_gen;

  switch (type) {
  case RND_TYPE_LINEAR:
    linear_rnd_gen.seed = seed>0?seed:linear_rnd_gen.seed;
    return _linear_lehmer_gen_real(&linear_rnd_gen);
    break;
  case RND_TYPE_COMBINED:
    /// Generate random number in Combined Linear Congruential
    combined_rnd_gen.gen1.seed = seed>0?seed:combined_rnd_gen.gen1.seed;
    return _combined_linear_gen_real(&combined_rnd_gen);
    break;
  case RND_TYPE_TAUSWORTHE:
  default:
    iprintf(LEVEL_WARNING, "Not support this type (%d) to generate random number\n", type);
    return 0;
  }
  return 0;
}

unsigned long irand_gen_range_random (int type, unsigned long from, unsigned long to) {
  unsigned long value = irand_gen_random(type);
  if (to <= from) {
    iprintf(LEVEL_WARNING, "irand failed to generate Random number: Range is not correct \n");
    return 0;
  }
  return (value % (to - from) + from);
}

/**
 * Initialize pseudo random generator
 * @return Error code (see more in def.h and error.h)
 */
int irand_init () {
  _linear_lehmer_gen_init(&linear_rnd_gen);
  _combined_linear_gen_init(&combined_rnd_gen);
  return SUCCESS;
}

/**
 * Psuedo random generator with seed
 * @param seed : seed of generator
 * @return integer pseudo random value
 */
int irand_new_seed (unsigned long seed) {
  if (seed == 0) {
    iprintf(LEVEL_WARNING, "Seed should not be 0. \n");
    return 0;
  }
  linear_rnd_gen.seed = seed;
  combined_rnd_gen.gen1.seed = seed;
  combined_rnd_gen.gen2.seed = seed;
  return SUCCESS;
}

/**
 * Psuedo random generator with random-selected seed
 * @return integer pseudo random value
 */
int irand_random_seed() {
  unsigned long seed = time(0) % 2009;
  irand_new_seed(seed);
  return SUCCESS;
}
