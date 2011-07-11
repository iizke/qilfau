/**
 * @file config.c
 * Some functions related to config structure
 *
 * @date Created on: May 24, 2011
 * @author iizke
 */

#include <string.h>

#include "error.h"
#include "random.h"
#include "parser.h"
#include "config.h"
#include "channelparser.h"

extern CONFIG conf;
extern NET_CONFIG netconf;
/**
 * Configure random distribution from parameters in RANDOM_CONFIG
 * @param rc : Random configuration
 * @return Error code (see more in def.h and error.h)
 */
int config_random_conf (RANDOM_CONF *rc) {
  struct mmpp_params *p = NULL;
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
  case RANDOM_CONST:
    random_dist_init_const(&rc->distribution, &rc->constval);
    break;
  case RANDOM_MMPP:
    p = malloc_gc(sizeof(struct mmpp_params));
    irand_mmpp_params_init (p, rc->from_file);
    rc->distribution.params = p;
    random_dist_init_mmpp(&rc->distribution, p);
    break;
  default:
    iprint(LEVEL_ERROR, "This type (%d) of random type is not supported\n", rc->type);
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
  config_random_conf(&conf->routing_conf);
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
  fclose(yyin);
  return SUCCESS;
}

CONFIG* netconfig_get_conf(NET_CONFIG *net, int cid) {
  return array_get(&net->configs, cid).pointer;
}

int netconfig_get_size(NET_CONFIG *net) {
  return net->configs.size;
}

int netconfig_traverse_arrival(NET_CONFIG *conf, int *qid) {
  int i = 0;
  for (i=(*qid); i<conf->configs.size; i++) {
    CONFIG *c = netconfig_get_conf(conf, i);
    if (c->arrival_conf.type != RANDOM_OTHER) {
      *qid = i;
      return TRUE;
    }
  }
  return FALSE;
}

int netconfig_init (NET_CONFIG *netconf, int n) {
  int i = 0;
  check_null_pointer(netconf);
  memset(netconf, 0, sizeof(CONFIG));
  array_setup(&netconf->configs, sizeof(CONFIG), n);
  linked_list_init(&netconf->channels);
  for (i=0; i<n; i++) {
    CONFIG *c = array_get(&netconf->configs, i).pointer;
    config_init(c);
  }
  return SUCCESS;
}

int netconfig_parse_nodes(char * f) {
  extern FILE *yyin;
  extern int yylex();
  extern int yyparse();
  extern NET_CONFIG netconf;
  CONFIG *config = netconfig_get_conf(&netconf, netconf.nnodes);
  if (!config)
    return ERR_POINTER_NULL;
  //This action should be done before calling parse_nodes
  config_init(&conf);
  //fclose(yyin);
  yyin = fopen(f, "r");
  yyparse();
  memcpy(config, &conf, sizeof(CONFIG));
  config_setup(config);
  netconf.nnodes++;
  fclose(yyin);
  return SUCCESS;
}

int netconfig_parse_channels(char * f) {
  extern FILE *nqin;
  extern int nqlex();
  extern int nqparse();
  extern NET_CONFIG netconf;
  LINKED_LIST *t = &netconf.channels;

  nqin = fopen(f, "r");
  nqparse();
  fclose(nqin);

  while (&netconf.channels != t->next) {
    CHANNEL_CONF *c = container_of(t->next, CHANNEL_CONF, list_node);
    config_random_conf(&c->delay);
    t = t->next;
  }
  return SUCCESS;
}
