%{ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "channelparser.h"
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
RANDOM_CONSTANT				return R_CONST;
RANDOM_OTHER				return R_OTHER;

link.begin					return LINK_BEGIN;
link.end					return LINK_END;
link.delay.type				return LINK_DELAY_TYPE;
link.delay.to				return LINK_DELAY_TO;
link.delay.from				return LINK_DELAY_FROM;
link.delay.lambda			return LINK_DELAY_LAMBDA;
link.delay.const			return LINK_DELAY_CONST;
link.source					return LINK_SRC;
link.destination			return LINK_DEST;
 
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