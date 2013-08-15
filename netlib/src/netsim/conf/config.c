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
#include "netsim/queue_babs.h"
#include "babs_parser.h"

extern CONFIG conf;
extern NET_CONFIG netconf;
extern BABSQ_CONFIG babs_conf;
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
  case RANDOM_MMPP_R:
    p = malloc_gc(sizeof(struct mmpp_r_params));
    irand_mmpp_r_params_init (p, rc->from_file);
    rc->distribution.params = p;
    random_dist_init_mmpp_r(&rc->distribution, p);
    break;
  case RANDOM_POISSON:
    random_dist_init_poisson(&rc->distribution, &rc->lambda);
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

/**
 * Initialize the structure of CONFIG
 * @param conf : configuration
 * @return Error code (see more in def.h and error.h)
 */
int babs_config_init (BABSQ_CONFIG *conf) {
  check_null_pointer(conf);
  memset(conf, 0, sizeof(BABSQ_CONFIG));
  conf->random_lib = LIB_RANDOM_IRAND;
  conf->stop_conf.queue_zero = STOP_QUEUE_NONZERO;
  return SUCCESS;
}

/**
 * Configure random distribution in user configuration
 * @param conf : User configuration
 * @return Error code (see more in def.h and error.h)
 */
int babs_config_setup (BABSQ_CONFIG *conf) {
  extern int librand;

  config_random_conf(&conf->arrival_conf);
  config_random_conf(&conf->service_conf);
  config_random_conf(&conf->burst_conf);
  librand = conf->random_lib;
  return SUCCESS;
}

static int babsq_config_print (BABSQ_CONFIG *conf) {
  check_null_pointer(conf);
  printf("BABS CONFIG: #threads = %d\n", conf->nthreads);
  printf("BABS CONFIG: protocol = %d - %d (BABSQ)\n", conf->protocol, PROTOCOL_BABSQ);
  printf("BABS CONFIG: randomlib = %d - irand %d - randlib %d\n", conf->random_lib, LIB_RANDOM_IRAND, LIB_RANDOM_RANDLIB);
  printf("BABS CONFIG: arrival_conf.type = %d\n", conf->arrival_conf.type);
  printf("BABS CONFIG: arrival_conf.lambda = %f\n", conf->arrival_conf.lambda);
  printf("BABS CONFIG: arrival_conf.from = %f\n", conf->arrival_conf.from);
  printf("BABS CONFIG: arrival_conf.to = %f\n", conf->arrival_conf.to);
  printf("BABS CONFIG: arrival_conf.prob = %f\n", conf->arrival_conf.prob);
  printf("----------------------------------------------\n");
  printf("BABS CONFIG: service_conf.type = %d\n", conf->service_conf.type);
  printf("BABS CONFIG: service_conf.lambda = %f\n", conf->service_conf.lambda);
  printf("BABS CONFIG: service_conf.from = %f\n", conf->service_conf.from);
  printf("BABS CONFIG: service_conf.to = %f\n", conf->service_conf.to);
  printf("BABS CONFIG: service_conf.prob = %f\n", conf->service_conf.prob);
  printf("----------------------------------------------\n");
  printf("BABS CONFIG: burst_conf.type = %d\n", conf->burst_conf.type);
  printf("BABS CONFIG: burst_conf.lambda = %f\n", conf->burst_conf.lambda);
  printf("BABS CONFIG: burst_conf.from = %f\n", conf->burst_conf.from);
  printf("BABS CONFIG: burst_conf.to = %f\n", conf->burst_conf.to);
  printf("BABS CONFIG: burst_conf.prob = %f\n", conf->burst_conf.prob);
  printf("----------------------------------------------\n");
  printf("BABS CONFIG: queue_conf.max_waiters = %d\n", conf->queue_conf.max_waiters);
  printf("BABS CONFIG: queue_conf.num_servers = %d\n", conf->queue_conf.num_servers);
  printf("BABS CONFIG: queue_conf.type = %d - BABSQ %d\n", conf->queue_conf.type, QUEUE_BURST_FIFO);
  printf("----------------------------------------------\n");
  printf("BABS CONFIG: stop_conf.max_arrival = %d\n", conf->stop_conf.max_arrival);
  printf("BABS CONFIG: stop_conf.max_time = %f\n", conf->stop_conf.max_time);
  printf("BABS CONFIG: stop_conf.queue_zero = %d\n", conf->stop_conf.queue_zero);
  return SUCCESS;
}
/**
 * Parse the configuration file for BABS Queue (for example: test.conf)
 * @param f : file name
 * @return : Error code (see more in def.h and error.h)
 */
int config_parse_babs_file(char * f) {
  extern FILE *babsin;
  extern int babslex();
  extern int babsparse();

  babs_config_init(&babs_conf);
  babsin = fopen(f, "r");
  babsparse();
  babs_config_setup(&babs_conf);
  fclose(babsin);
  // print out config structure
  //babsq_config_print(&babs_conf);
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
  array_init(&netconf->configs, sizeof(CONFIG), n);
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
