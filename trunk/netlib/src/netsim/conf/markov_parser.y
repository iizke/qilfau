
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#include "error.h"
//long debug = 0;
//CONFIG conf;
%}

%locations
%pure_parser
 
%union {char* str; double val;}

%token EQ
%token ENDLINE
%token VAR
%token <val> NUM
%token <str> STRING
%type <val> exp
%type <str> vexp
%left '+' '-'
%left '*' '/'
%% /* Grammar rules and actions follow */
input:    /* empty */
        | input line 
;

line:	  exp ENDLINE { printf("resultl = %f\n", $1); }
    |   vexp ENDLINE
		|   ENDLINE
;

exp:    NUM  
    |   exp '+' exp { $$=$1+$3; }
    |   exp '-' exp { $$=$1-$3; }
    |   exp '*' exp { $$=$1*$3; }
    |   exp '/' exp { $$=$1/$3; }
    |   '(' exp ')' { $$=$2;}
;

vexp:   VAR {printf("x");}
    |   vexp '+' vexp { printf("x+x"); }
;

%%

int mperror (char *s)  /* Called by yyparse on error */
{
  printf ("yyparse error: %s\n", s);
  exit(-1);
  return 0;
}

int mpwrap(void* no_use) { return 1;  }

int result = 0;

/*
int main (int argc, char** argv) {
  extern FILE *yyin; 
  extern int yylex();
  extern int yyparse();

  //printf("Open file : %s\n", argv[1]);
  yyin = fopen("calc.txt", "r");
  yyparse();
  fclose(yyin); 
  return 0;
}

*/