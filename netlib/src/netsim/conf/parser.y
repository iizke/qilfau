
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "error.h"
#include "../queues/queue_man.h"

#ifndef DEF_DEBUG
#define DEF_DEBUG
long debug = 0;
#else
extern long debug;
#endif
CONFIG conf;
%}

%locations
%pure_parser
 
%union {char* str; int ival; double dval;}

%token ARRIVAL_TYPE
%token ARRIVAL_FROM
%token ARRIVAL_TO
%token ARRIVAL_LAMBDA
%token ARRIVAL_SAVETO
%token ARRIVAL_LOADFROM

%token QUEUE_KIND
%token FIFO_QUEUE
%token QUEUE_MAXLENGTH
%token QUEUE_SERVERS
%token SERVICE_TYPE;
%token SERVICE_LAMBDA;
%token SERVICE_TO;
%token SERVICE_FROM;
%token SERVICE_SAVETO;
%token SERVICE_LOADFROM;

%token DEPARTURE_SAVETO;

%token STOP_MAXTIME;
%token STOP_MAXARRIVAL;
%token STOP_QUEUEZERO;

%token NTHREADS;
%token NODETYPE;
%token SOURCE;
%token SINK;
%token TRANSIT;

%token CSMA_NSTATIONS;
%token CSMA_PROB;
%token CSMA_MAXBACKOFF;
%token CSMA_COLLISION;
%token CSMA_SLOTTIME;

%token PROTOCOL;
%token PROTO_CSMA;
%token PROTO_ONE_QUEUE;
%token PROTO_NET_QUEUE;

%token LIBS_RANDOM;
%token LIB_RAND_IRAND;
%token LIB_RAND_RANDLIB;

%token R_MARKOV;
%token R_UNF;
%token R_FILE;
%token R_MMPP;
%token R_MMPP_R;
%token R_OTHER;

%token DEBUG_ERROR;
%token DEBUG_WARNING;
%token DEBUG_INFO;

%token YES;
%token NO;
%token EQ;
%token ENDLINE;
%token <dval> REALNUM;
%token <ival> INTNUM;
%token <str> STRING;

%% /* Grammar rules and actions follow */
input:    /* empty here*/
        | input line
;

line:	  exp ENDLINE
		| ENDLINE
;

exp:      ARRIVAL_TYPE EQ INTNUM { conf.arrival_conf.type = $3; }
		| ARRIVAL_TYPE EQ R_MARKOV { conf.arrival_conf.type = RANDOM_MARKOVIAN; }
		| ARRIVAL_TYPE EQ R_UNF { conf.arrival_conf.type = RANDOM_UNIFORM; }
		| ARRIVAL_TYPE EQ R_FILE { conf.arrival_conf.type = RANDOM_FILE; }
		| ARRIVAL_TYPE EQ R_MMPP { conf.arrival_conf.type = RANDOM_MMPP; }
		| ARRIVAL_TYPE EQ R_MMPP_R { conf.arrival_conf.type = RANDOM_MMPP_R; }
		| ARRIVAL_TYPE EQ R_OTHER { conf.arrival_conf.type = RANDOM_OTHER; }
		| ARRIVAL_FROM EQ INTNUM { conf.arrival_conf.from = $3; }
		| ARRIVAL_TO EQ INTNUM { conf.arrival_conf.to = $3; }
		| ARRIVAL_LAMBDA EQ REALNUM { conf.arrival_conf.lambda = $3; }
		| ARRIVAL_LAMBDA EQ INTNUM { conf.arrival_conf.lambda = $3; }
		| ARRIVAL_SAVETO EQ STRING { conf.arrival_conf.to_file = fopen($3, "w+"); }
		| ARRIVAL_LOADFROM EQ STRING { conf.arrival_conf.from_file = fopen($3, "r"); }
		
		| QUEUE_MAXLENGTH EQ INTNUM { conf.queue_conf.max_waiters = $3; }
		| QUEUE_KIND EQ INTNUM { conf.queue_conf.type = $3; }
		| QUEUE_KIND EQ FIFO_QUEUE { conf.queue_conf.type = QUEUE_FIFO; }
		| QUEUE_SERVERS EQ INTNUM { conf.queue_conf.num_servers = $3; }
		| SERVICE_TYPE EQ INTNUM {conf.service_conf.type = $3; }
		| SERVICE_TYPE EQ R_MARKOV {conf.service_conf.type = RANDOM_MARKOVIAN; }
		| SERVICE_TYPE EQ R_MMPP {conf.service_conf.type = RANDOM_MMPP; }
		| SERVICE_TYPE EQ R_MMPP_R {conf.service_conf.type = RANDOM_MMPP_R; }
		| SERVICE_TYPE EQ R_UNF {conf.service_conf.type = RANDOM_UNIFORM; }
		| SERVICE_TYPE EQ R_OTHER {conf.service_conf.type = RANDOM_OTHER; }
		| SERVICE_FROM EQ REALNUM { conf.service_conf.from = $3; }
		| SERVICE_TO EQ REALNUM { conf.service_conf.to = $3; }
		| SERVICE_FROM EQ INTNUM { conf.service_conf.from = $3; }
		| SERVICE_TO EQ INTNUM { conf.service_conf.to = $3; }
		| SERVICE_LAMBDA EQ REALNUM { conf.service_conf.lambda = $3; }
		| SERVICE_LAMBDA EQ INTNUM { conf.service_conf.lambda = $3; }
		| SERVICE_SAVETO EQ STRING {conf.service_conf.to_file = fopen($3, "w+");}
		| SERVICE_LOADFROM EQ STRING {conf.service_conf.from_file = fopen($3, "r");}
		
		| DEPARTURE_SAVETO EQ STRING { conf.queue_conf.out_file = fopen($3, "w+");}
		| STOP_MAXTIME EQ INTNUM { conf.stop_conf.max_time = $3; }
		| STOP_MAXARRIVAL EQ INTNUM { conf.stop_conf.max_arrival = $3; }
		| STOP_QUEUEZERO EQ YES {conf.stop_conf.queue_zero = STOP_QUEUE_ZERO; }
		| STOP_QUEUEZERO EQ NO {conf.stop_conf.queue_zero = STOP_QUEUE_NONZERO; }
		
		| LIBS_RANDOM EQ LIB_RAND_IRAND {conf.random_lib = LIB_RANDOM_IRAND; }
		| LIBS_RANDOM EQ LIB_RAND_RANDLIB {conf.random_lib = LIB_RANDOM_RANDLIB; }
		
		| DEBUG_ERROR EQ YES {debug |= LEVEL_ERROR; }
		| DEBUG_ERROR EQ NO {debug &= ~LEVEL_ERROR;}
		| DEBUG_WARNING EQ YES {debug |= LEVEL_WARNING;}
		| DEBUG_WARNING EQ NO {debug &= ~LEVEL_WARNING;}
		| DEBUG_INFO EQ YES {debug |= LEVEL_INFO;}
		| DEBUG_INFO EQ NO {debug &= ~LEVEL_INFO;}
		
		| CSMA_NSTATIONS EQ INTNUM {conf.csma_conf.nstations = $3;}
		| CSMA_PROB EQ REALNUM {conf.csma_conf.persistent_conf.prob = $3; conf.csma_conf.persistent_conf.type = RANDOM_BERNOULLI;}
		| CSMA_SLOTTIME EQ REALNUM {conf.csma_conf.slot_time = $3;}
		| CSMA_COLLISION EQ REALNUM {conf.csma_conf.collision_time = $3;}
		| CSMA_MAXBACKOFF EQ INTNUM {conf.csma_conf.backoff_conf.to = $3; conf.csma_conf.backoff_conf.from = 1; conf.csma_conf.backoff_conf.type = RANDOM_UNIFORM;}

		| PROTOCOL EQ PROTO_CSMA {conf.protocol = PROTOCOL_CSMA; }
		| PROTOCOL EQ PROTO_ONE_QUEUE {conf.protocol = PROTOCOL_ONE_QUEUE;}
		| PROTOCOL EQ PROTO_NET_QUEUE {conf.protocol = PROTOCOL_NET_QUEUE;}
		
		| NTHREADS EQ INTNUM { conf.nthreads = $3; }
		| NODETYPE EQ SOURCE { conf.nodetype = NODE_SOURCE; }
		| NODETYPE EQ SINK { conf.nodetype = NODE_SINK; }
		| NODETYPE EQ TRANSIT { conf.nodetype = NODE_TRANSIT; }
;
%%

int yyerror (char *s)  /* Called by yyparse on error */
{
  printf ("yyparse error: %s\n", s);
  exit(-1);
  return 0;
}

int yywrap(void* no_use) { return 1;  }
