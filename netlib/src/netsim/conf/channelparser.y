
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "error.h"
#include "netsim/netqueue.h"
//long debug = 0;
NET_CONFIG netconf;
CHANNEL_CONF *channel = NULL; 
%}

%locations
%pure_parser
 
%union {char* str; int ival; double dval;}

%token R_MARKOV;
%token R_UNF;
%token R_FILE;
%token R_OTHER;
%token R_CONST;

%token LINK_BEGIN
%token LINK_END
%token LINK_DELAY_TYPE
%token LINK_DELAY_TO
%token LINK_DELAY_FROM
%token LINK_DELAY_LAMBDA
%token LINK_DELAY_CONST
%token LINK_SRC
%token LINK_DEST

%token YES;
%token NO;
%token EQ;
%token ENDLINE;
%token <dval> REALNUM;
%token <ival> INTNUM;
%token <str> STRING;

%% /* Grammar rules and actions follow */
input:    /* empty */
        | input link
;

link:	LINK_BEGIN {channel = malloc_gc(sizeof(CHANNEL_CONF));}
		lines
		LINK_END {linked_list_insert(&netconf.channels, &channel->list_node);}
;

lines:    line
		| lines line
;

line:	  exp ENDLINE
;

exp:      LINK_DELAY_TYPE EQ INTNUM { channel->delay.type = $3; }
		| LINK_DELAY_TYPE EQ R_MARKOV { channel->delay.type = RANDOM_MARKOVIAN; }
		| LINK_DELAY_TYPE EQ R_CONST { channel->delay.type = RANDOM_CONST;}
		| LINK_DELAY_TYPE EQ R_UNF { channel->delay.type = RANDOM_UNIFORM; }
		| LINK_DELAY_TYPE EQ R_FILE { channel->delay.type = RANDOM_FILE; }
		| LINK_DELAY_TYPE EQ R_OTHER { channel->delay.type = RANDOM_OTHER; }
		| LINK_DELAY_FROM EQ INTNUM { channel->delay.from = $3; }
		| LINK_DELAY_TO EQ INTNUM { channel->delay.to = $3; }
		| LINK_DELAY_LAMBDA EQ REALNUM { channel->delay.lambda = $3; }
		| LINK_DELAY_CONST EQ REALNUM { channel->delay.constval = $3;}
		| LINK_DELAY_CONST EQ INTNUM { channel->delay.constval = $3;}
		| LINK_DELAY_LAMBDA EQ INTNUM { channel->delay.lambda = $3; }
		| LINK_SRC EQ INTNUM { channel->src = $3;}
		| LINK_DEST EQ INTNUM { channel->dest = $3;}
;
%%

int nqerror (char *s)  /* Called by yyparse on error */
{
  printf ("nqparse error: %s\n", s);
  exit(-1);
  return 0;
}

int nqwrap(void* no_use) { return 1;  }

