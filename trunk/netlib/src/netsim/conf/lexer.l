%{ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
%} 
%option bison-bridge bison-locations

DIGIT						[0-9]
SIGN						"-"
CHAR						[a-zA-Z_/]
COMMENT 					"//"
POINT						"."
FCHAR						({CHAR}|{POINT}|{DIGIT})
%% 
\/\/[^\n]*\n				; 

nodetype					return NODETYPE;
SOURCE						return SOURCE;
SINK						return SINK;
TRANSIT						return TRANSIT;

RANDOM_MARKOV				return R_MARKOV;
RANDOM_UNIFORM				return R_UNF;
RANDOM_FILE					return R_FILE;
RANDOM_OTHER				return R_OTHER;
RANDOM_MMPP					return R_MMPP;
RANDOM_MMPP_RULES			return R_MMPP_R;

arrival.type				return ARRIVAL_TYPE;
arrival.lambda				return ARRIVAL_LAMBDA;
arrival.from				return ARRIVAL_FROM;
arrival.to					return ARRIVAL_TO;
arrival.saveto				return ARRIVAL_SAVETO;
arrival.loadfrom			return ARRIVAL_LOADFROM;

queue.maxlength				return QUEUE_MAXLENGTH;
queue.servers				return QUEUE_SERVERS;
queue.type					return QUEUE_KIND;
QUEUE_FIFO					return FIFO_QUEUE;
service.type				return SERVICE_TYPE;
service.lambda				return SERVICE_LAMBDA;
service.to					return SERVICE_TO;
service.from				return SERVICE_FROM;
service.saveto				return SERVICE_SAVETO;
service.loadfrom			return SERVICE_LOADFROM;
departure.saveto			return DEPARTURE_SAVETO;

stop.maxtime				return STOP_MAXTIME;
stop.maxarrival				return STOP_MAXARRIVAL;
stop.queuezero				return STOP_QUEUEZERO;

csma.nstations				return CSMA_NSTATIONS;
csma.probability			return CSMA_PROB;
csma.slottime				return CSMA_SLOTTIME;
csma.collision				return CSMA_COLLISION;
csma.maxbackoff				return CSMA_MAXBACKOFF;

protocol					return PROTOCOL;
ONE_QUEUE					return PROTO_ONE_QUEUE;
CSMA						return PROTO_CSMA;
NET_QUEUE					return PROTO_NET_QUEUE;
nthreads					return NTHREADS;

debug.error					return DEBUG_ERROR;
debug.warning				return DEBUG_WARNING;
debug.info					return DEBUG_INFO;
libs.random					return LIBS_RANDOM;
LIB_RANDOM_IRAND			return LIB_RAND_IRAND;
LIB_RANDOM_RANDLIB			return LIB_RAND_RANDLIB;
=							return EQ;
;							return ENDLINE;
yes							return YES;
no							return NO;
{SIGN}{DIGIT}+{POINT}{DIGIT}+		{yylval->dval = atof(yytext); return REALNUM;}
{DIGIT}+{POINT}{DIGIT}+				{yylval->dval = atof(yytext); return REALNUM;}
{SIGN}{DIGIT}+						{yylval->ival = atoi(yytext); return INTNUM;}
{DIGIT}+							{yylval->ival = atoi(yytext); return INTNUM;}
{CHAR}{FCHAR}*						{yylval->str = strdup(yytext); return STRING;}


[ \t\n]+        			;       /* ignore whitespace */ 

%% 

#ifdef YYLEX_PARAM
#define YYLEX1 yylex_fast (&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX1 yylex_fast (&yylval, &yylloc)
#endif

#ifndef YY_DECL1
#define YY_DECL_IS_OURS1 1
extern int yylex_fast(YYSTYPE * yylval_param,YYLTYPE * yylloc_param );
#define YY_DECL1 int yylex_fast(YYSTYPE * yylval_param, YYLTYPE * yylloc_param )
#endif /* !YY_DECL */

YY_DECL1 {
  return 0;
}
