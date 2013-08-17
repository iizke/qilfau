
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../queue_babs.h"
#include "error.h"
#include "../queues/queue_man.h"
#include "random.h"

//#ifndef DEF_DEBUG
//#define DEF_DEBUG
//long debug = 0;
//#else
extern long debug;
//#endif

BABSQ_CONFIG babs_conf;
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
%token ARRIVAL_MEAN
%token ARRIVAL_SDEV

%token BURST_TYPE
%token BURST_FROM
%token BURST_TO
%token BURST_LAMBDA
%token BURST_SAVETO
%token BURST_LOADFROM
%token BURST_MEAN
%token BURST_SDEV

%token QUEUE_KIND
%token BURST_FIFO_QUEUE
%token QUEUE_MAXLENGTH
%token QUEUE_SERVERS
%token SERVICE_TYPE;
%token SERVICE_LAMBDA;
%token SERVICE_TO;
%token SERVICE_FROM;
%token SERVICE_SAVETO;
%token SERVICE_LOADFROM;
%token SERVICE_MEAN;
%token SERVICE_SDEV;

%token DEPARTURE_SAVETO;

%token STOP_MAXTIME;
%token STOP_MAXARRIVAL;
%token STOP_QUEUEZERO;

%token NTHREADS;

%token PROTOCOL;
%token PROTO_BABSQ;

%token LIBS_RANDOM;
%token LIB_RAND_IRAND;
%token LIB_RAND_RANDLIB;

%token R_MARKOV;
%token R_UNF;
%token R_FILE;
%token R_MMPP;
%token R_MMPP_R;
%token R_POISSON;
%token R_OTHER;
%token R_NORMAL;

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

exp:      ARRIVAL_TYPE EQ INTNUM { babs_conf.arrival_conf.type = $3; }
		| ARRIVAL_TYPE EQ R_MARKOV { babs_conf.arrival_conf.type = RANDOM_MARKOVIAN; }
		| ARRIVAL_TYPE EQ R_UNF { babs_conf.arrival_conf.type = RANDOM_UNIFORM; }
		| ARRIVAL_TYPE EQ R_FILE { babs_conf.arrival_conf.type = RANDOM_FILE; }
		| ARRIVAL_TYPE EQ R_MMPP { babs_conf.arrival_conf.type = RANDOM_MMPP; }
		| ARRIVAL_TYPE EQ R_MMPP_R { babs_conf.arrival_conf.type = RANDOM_MMPP_R; }
		| ARRIVAL_TYPE EQ R_OTHER { babs_conf.arrival_conf.type = RANDOM_OTHER; }
		| ARRIVAL_TYPE EQ R_NORMAL { babs_conf.arrival_conf.type = RANDOM_NORMAL; random_dist_init_normal_empty(&babs_conf.arrival_conf.distribution); }
		| ARRIVAL_FROM EQ INTNUM { babs_conf.arrival_conf.from = $3; }
		| ARRIVAL_TO EQ INTNUM { babs_conf.arrival_conf.to = $3; }
		| ARRIVAL_LAMBDA EQ REALNUM { babs_conf.arrival_conf.lambda = $3; }
		| ARRIVAL_LAMBDA EQ INTNUM { babs_conf.arrival_conf.lambda = $3; }
		| ARRIVAL_SAVETO EQ STRING { babs_conf.arrival_conf.to_file = fopen($3, "w+"); }
		| ARRIVAL_LOADFROM EQ STRING { babs_conf.arrival_conf.from_file = fopen($3, "r"); }
		| ARRIVAL_MEAN EQ REALNUM { if (random_dist_normal_set_mean(&babs_conf.arrival_conf.distribution, $3) < 0) exit(1); }
		| ARRIVAL_MEAN EQ INTNUM { if (random_dist_normal_set_mean(&babs_conf.arrival_conf.distribution, $3) < 0) exit(1); }
		| ARRIVAL_SDEV EQ REALNUM { if (random_dist_normal_set_sdev(&babs_conf.arrival_conf.distribution, $3) < 0) exit(1); }
		| ARRIVAL_SDEV EQ INTNUM { if (random_dist_normal_set_sdev(&babs_conf.arrival_conf.distribution, $3) < 0) exit(1); }
		| BURST_TYPE EQ R_POISSON { babs_conf.burst_conf.type = RANDOM_POISSON; }
	        | BURST_TYPE EQ R_UNF { babs_conf.burst_conf.type = RANDOM_UNIFORM; }
        	| BURST_TYPE EQ R_FILE { babs_conf.burst_conf.type = RANDOM_FILE; }
	        | BURST_TYPE EQ R_MMPP { babs_conf.burst_conf.type = RANDOM_MMPP; }
        	| BURST_TYPE EQ R_MMPP_R { babs_conf.burst_conf.type = RANDOM_MMPP_R; }
	        | BURST_TYPE EQ R_OTHER { babs_conf.burst_conf.type = RANDOM_OTHER; }
		| BURST_TYPE EQ R_NORMAL { babs_conf.burst_conf.type = RANDOM_NORMAL; random_dist_init_normal_empty(&babs_conf.burst_conf.distribution);}
	        | BURST_FROM EQ INTNUM { babs_conf.burst_conf.from = $3; }
        	| BURST_TO EQ INTNUM { babs_conf.burst_conf.to = $3; }
		| BURST_LAMBDA EQ REALNUM { babs_conf.burst_conf.lambda = $3; }
		| BURST_LAMBDA EQ INTNUM { babs_conf.burst_conf.lambda = $3; }
		| BURST_SAVETO EQ STRING { babs_conf.burst_conf.to_file = fopen($3, "w+"); }
		| BURST_LOADFROM EQ STRING { babs_conf.burst_conf.from_file = fopen($3, "r"); }
		| BURST_MEAN EQ REALNUM { if (random_dist_normal_set_mean (&babs_conf.burst_conf.distribution, $3) < 0) exit(1); }
		| BURST_MEAN EQ INTNUM { if (random_dist_normal_set_mean (&babs_conf.burst_conf.distribution, $3) < 0) exit(1); }
		| BURST_SDEV EQ REALNUM { if (random_dist_normal_set_sdev (&babs_conf.burst_conf.distribution, $3) < 0) exit(1); }
		| BURST_SDEV EQ INTNUM { if (random_dist_normal_set_sdev (&babs_conf.burst_conf.distribution, $3) < 0) exit(1); }
		| QUEUE_MAXLENGTH EQ INTNUM { babs_conf.queue_conf.max_waiters = $3; }
		| QUEUE_KIND EQ INTNUM { babs_conf.queue_conf.type = $3; }
		| QUEUE_KIND EQ BURST_FIFO_QUEUE { babs_conf.queue_conf.type = QUEUE_BURST_FIFO; }
		| QUEUE_SERVERS EQ INTNUM { babs_conf.queue_conf.num_servers = $3; }
		| SERVICE_TYPE EQ INTNUM { babs_conf.service_conf.type = $3; }
		| SERVICE_TYPE EQ R_MARKOV {babs_conf.service_conf.type = RANDOM_MARKOVIAN; }
		| SERVICE_TYPE EQ R_MMPP {babs_conf.service_conf.type = RANDOM_MMPP; }
		| SERVICE_TYPE EQ R_MMPP_R {babs_conf.service_conf.type = RANDOM_MMPP_R; }
		| SERVICE_TYPE EQ R_UNF {babs_conf.service_conf.type = RANDOM_UNIFORM; }
		| SERVICE_TYPE EQ R_OTHER {babs_conf.service_conf.type = RANDOM_OTHER; }
		| SERVICE_TYPE EQ R_NORMAL {babs_conf.service_conf.type = RANDOM_NORMAL; random_dist_init_normal_empty(&babs_conf.service_conf.distribution);}
		| SERVICE_FROM EQ REALNUM { babs_conf.service_conf.from = $3; }
		| SERVICE_TO EQ REALNUM { babs_conf.service_conf.to = $3; }
		| SERVICE_FROM EQ INTNUM { babs_conf.service_conf.from = $3; }
		| SERVICE_TO EQ INTNUM { babs_conf.service_conf.to = $3; }
		| SERVICE_LAMBDA EQ REALNUM { babs_conf.service_conf.lambda = $3; }
		| SERVICE_LAMBDA EQ INTNUM { babs_conf.service_conf.lambda = $3; }
		| SERVICE_SAVETO EQ STRING {babs_conf.service_conf.to_file = fopen($3, "w+");}
		| SERVICE_LOADFROM EQ STRING {babs_conf.service_conf.from_file = fopen($3, "r");}
		| SERVICE_MEAN EQ REALNUM { if (random_dist_normal_set_mean (&babs_conf.service_conf.distribution, $3) < 0) exit(1); }
                | SERVICE_MEAN EQ INTNUM { if (random_dist_normal_set_mean (&babs_conf.service_conf.distribution, $3) < 0) exit(1); }
                | SERVICE_SDEV EQ REALNUM { if (random_dist_normal_set_sdev (&babs_conf.service_conf.distribution, $3) < 0) exit(1); }
                | SERVICE_SDEV EQ INTNUM { if (random_dist_normal_set_sdev (&babs_conf.service_conf.distribution, $3) < 0) exit(1); }		
		| DEPARTURE_SAVETO EQ STRING { babs_conf.queue_conf.out_file = fopen($3, "w+");}
		| STOP_MAXTIME EQ INTNUM { babs_conf.stop_conf.max_time = $3; }
		| STOP_MAXARRIVAL EQ INTNUM { babs_conf.stop_conf.max_arrival = $3; }
		| STOP_QUEUEZERO EQ YES {babs_conf.stop_conf.queue_zero = STOP_QUEUE_ZERO; }
		| STOP_QUEUEZERO EQ NO {babs_conf.stop_conf.queue_zero = STOP_QUEUE_NONZERO; }
		
		| LIBS_RANDOM EQ LIB_RAND_IRAND {babs_conf.random_lib = LIB_RANDOM_IRAND; }
		| LIBS_RANDOM EQ LIB_RAND_RANDLIB {babs_conf.random_lib = LIB_RANDOM_RANDLIB; }
		
		| DEBUG_ERROR EQ YES {debug |= LEVEL_ERROR; }
		| DEBUG_ERROR EQ NO {debug &= ~LEVEL_ERROR;}
		| DEBUG_WARNING EQ YES {debug |= LEVEL_WARNING;}
		| DEBUG_WARNING EQ NO {debug &= ~LEVEL_WARNING;}
		| DEBUG_INFO EQ YES {debug |= LEVEL_INFO;}
		| DEBUG_INFO EQ NO {debug &= ~LEVEL_INFO;}
		
		| PROTOCOL EQ PROTO_BABSQ {babs_conf.protocol = PROTOCOL_BABSQ; }
		
		| NTHREADS EQ INTNUM { babs_conf.nthreads = $3; }
;
%%

int babserror (char *s)  /* Called by yyparse on error */
{
  printf ("babs_yyparse error: %s\n", s);
  exit(-1);
  return 0;
}

int babswrap(void* no_use) { return 1;  }
