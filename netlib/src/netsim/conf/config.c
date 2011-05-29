/**
 * @file config.c
 * Some functions related to config structure
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <string.h>

#include "../../error.h"
#include "../../random.h"
#include "parser.h"
#include "config.h"

extern CONFIG conf;

/**
 * Configure random distribution from parameters in RANDOM_CONFIG
 * @param rc : Random configuration
 * @return Error code (see more in def.h and error.h)
 */
int config_random_conf (RANDOM_CONF *rc) {
  switch (rc->type) {
  case RANDOM_MARKOVIAN:
    random_dist_init_exp0(&rc->distribution, &rc->lambda);
    break;
  case RANDOM_UNIFORM:
    random_dist_init_uniform1(&rc->distribution, rc->from, rc->to);
    break;
  case RANDOM_FILE:
    random_dist_init_file0(&rc->distribution, rc->from_file);
    break;
  case RANDOM_BERNOULLI:
    random_dist_init_bernoulli0(&rc->distribution, &rc->prob);
    break;
  default:
    iprintf(LEVEL_ERROR, "This type (%d) of random type is not supported\n", rc->type);
    return ERR_RANDOM_TYPE_FAIL;
  }
  return SUCCESS;
}

/**
 * Initialize the structure of CONFIG
 * @param conf : configuration
 * @return Error code (see more in def.h and error.h)
 */
int config_init (CONFIG *conf) {
  check_null_pointer(conf);
  memset(conf, 0, sizeof(CONFIG));
  conf->random_lib = LIB_RANDOM_IRAND;
  conf->stop_conf.queue_zero = STOP_QUEUE_ZERO;
  return SUCCESS;
}

/**
 * Configure random distribution in user configuration
 * @param conf : User configuration
 * @return Error code (see more in def.h and error.h)
 */
int config_setup (CONFIG *conf) {
  extern int librand;

  config_random_conf(&conf->arrival_conf);
  config_random_conf(&conf->service_conf);
  config_random_conf(&conf->csma_conf.backoff_conf);
  config_random_conf(&conf->csma_conf.persistent_conf);
  librand = conf->random_lib;
  return SUCCESS;
}

/**
 * Parse the configuration file (for example: test.conf)
 * @param f : file name
 * @return : Error code (see more in def.h and error.h)
 */
int config_parse_file(char * f) {
  extern FILE *yyin;
  extern int yylex();
  extern int yyparse();

  config_init(&conf);
  yyin = fopen(f, "r");
  yyparse();
  config_setup(&conf);

  return SUCCESS;
}
