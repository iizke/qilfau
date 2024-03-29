%{ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "babs_parser.h"
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

RANDOM_MARKOV				return R_MARKOV;
RANDOM_UNIFORM				return R_UNF;
RANDOM_FILE					return R_FILE;
RANDOM_OTHER				return R_OTHER;
RANDOM_MMPP					return R_MMPP;
RANDOM_MMPP_RULES			return R_MMPP_R;
RANDOM_POISSON				return R_POISSON;
RANDOM_NORMAL				return R_NORMAL;

arrival.type				return ARRIVAL_TYPE;
arrival.lambda				return ARRIVAL_LAMBDA;
arrival.from				return ARRIVAL_FROM;
arrival.to					return ARRIVAL_TO;
arrival.saveto				return ARRIVAL_SAVETO;
arrival.loadfrom			return ARRIVAL_LOADFROM;
arrival.mean				return ARRIVAL_MEAN;
arrival.sdev				return ARRIVAL_SDEV;

burst.type					return BURST_TYPE;
burst.lambda				return BURST_LAMBDA;
burst.from					return BURST_FROM;
burst.to					return BURST_TO;
burst.saveto				return BURST_SAVETO;
burst.loadfrom				return BURST_LOADFROM;
burst.mean					return BURST_MEAN;
burst.sdev					return BURST_SDEV;

queue.maxlength				return QUEUE_MAXLENGTH;
queue.servers				return QUEUE_SERVERS;
queue.type					return QUEUE_KIND;
queue.window				return QUEUE_WINDOW;
QUEUE_BURST_FIFO			return BURST_FIFO_QUEUE;
QUEUE_BURST_SCHED1			return BURST_SCHED1_QUEUE;
QUEUE_BURST_SCHEDWIN		return BURST_SCHEDWIN_QUEUE;

service.type				return SERVICE_TYPE;
service.lambda				return SERVICE_LAMBDA;
service.to					return SERVICE_TO;
service.from				return SERVICE_FROM;
service.saveto				return SERVICE_SAVETO;
service.loadfrom			return SERVICE_LOADFROM;
service.mean				return SERVICE_MEAN;
service.sdev				return SERVICE_SDEV;

departure.saveto			return DEPARTURE_SAVETO;

stop.maxtime				return STOP_MAXTIME;
stop.maxarrival				return STOP_MAXARRIVAL;
stop.queuezero				return STOP_QUEUEZERO;

protocol				return PROTOCOL;
BABS_QUEUE				return PROTO_BABSQ;
nthreads				return NTHREADS;

debug.error				return DEBUG_ERROR;
debug.warning				return DEBUG_WARNING;
debug.info				return DEBUG_INFO;
libs.random				return LIBS_RANDOM;
LIB_RANDOM_IRAND			return LIB_RAND_IRAND;
LIB_RANDOM_RANDLIB			return LIB_RAND_RANDLIB;
=					return EQ;
;					return ENDLINE;
yes					return YES;
no					return NO;
{SIGN}{DIGIT}+{POINT}{DIGIT}+		{yylval->dval = atof(yytext); return REALNUM;}
{DIGIT}+{POINT}{DIGIT}+			{yylval->dval = atof(yytext); return REALNUM;}
{SIGN}{DIGIT}+				{yylval->ival = atoi(yytext); return INTNUM;}
{DIGIT}+				{yylval->ival = atoi(yytext); return INTNUM;}
{CHAR}{FCHAR}*				{yylval->str = strdup(yytext); return STRING;}


[ \t\n]+        			;       /* ignore whitespace */ 

%% 

