%{ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "markov_parser.h"
%} 
%option bison-bridge bison-locations

DIGIT						[0-9]
SIGN						"-"
CHAR						[a-zA-Z_]
COMMENT 					"//"
POINT						"."
FCHAR						({CHAR}|{POINT}|{DIGIT})
%% 

=									            {return EQ;}
;									            {return ENDLINE;}

{DIGIT}+{POINT}{DIGIT}+				{yylval->val = atof(yytext); return NUM;}
{DIGIT}+							        {yylval->val = atoi(yytext); return NUM;}
x                             {return VAR;}
{CHAR}{FCHAR}*						    {yylval->str = strdup(yytext); return STRING;}

[ \t\n]+                      ;       /* ignore whitespace */

.                             {return yytext[0];}
%% 
