
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "vexpr.h"
//#include "error.h"
//long debug = 0;
//CONFIG conf;
VEXPR vexpr;
%}

%locations
%pure_parser
 
%union {char* str; double val;}

%token EQ
%token ENDLINE
%token VAR
%token PP
%token IF
%token THEN
%token ANDO
%token <val> NUM
%token <str> STRING
/* %type <val> exp */
%type <str> vexp
/* %type <str> poison */

%left ANDO
%left '<' '>'
%left EQ '?'
%left '+' '-'
%left '*' '/'
%% /* Grammar rules and actions follow */
input:    /* empty */
        | input line 
;

line:		vexp ENDLINE { vexpr_assign_value(&vexpr, $1); 
							VEXPR_NODE *var = vexpr_get_var(&vexpr,'x');
							printf("result = %f\n", vexpr_calc(&vexpr));
							if (var) printf("x = %f\n", var->val); }
    	|   ENDLINE
;

vexp:   VAR {$$ = vexpr_declare_varid(&vexpr, 'x', 1);}
	| 	NUM {$$ = vexpr_node_const($1);}
    |   vexp '+' vexp {$$ = vexpr_node_formular(VEXPR_OP_PLUS, $1, $3);}
    |   vexp '-' vexp {$$ = vexpr_node_formular(VEXPR_OP_MINUS, $1, $3);}
    |   vexp '*' vexp {$$ = vexpr_node_formular(VEXPR_OP_MUL, $1, $3);}
    |   vexp '/' vexp {$$ = vexpr_node_formular(VEXPR_OP_DIV, $1, $3);}
    |   vexp '>' vexp {$$ = vexpr_node_formular(VEXPR_OP_GT, $1, $3);}
    |   vexp '<' vexp {$$ = vexpr_node_formular(VEXPR_OP_LT, $1, $3);}
    |   '(' vexp ')' {$$ = $2;}
    |	vexp '?' vexp {$$ = vexpr_node_formular(VEXPR_OP_INFER, $1, $3);}
    |	vexp ANDO vexp {$$ = vexpr_node_formular(VEXPR_OP_ANDO, $1, $3);}
    |	vexp EQ vexp {$$ = vexpr_node_formular(VEXPR_OP_ASSIGN, $1, $3);}
;
/*
poison: PP '[' NUM ']' EQ NUM {printf("Poison rate \n");}
;
*/
%%

int mperror (char *s)  /* Called by yyparse on error */
{
  printf ("yyparse error: %s\n", s);
  exit(-1);
  return 0;
}

int mpwrap(void* no_use) { return 1;  }

int result = 0;


int main_markov_parser (int argc, char** argv) {
  extern FILE *mpin; 
  extern int mplex();
  extern int mpparse();
  
  //trash_init();
  vexpr_init(&vexpr);
  printf("Testing markov_parser \n");
  mpin = fopen("/home/iizke/projects/netlib/src/netsim/conf/calc.txt", "r");
  mpparse();
  fclose(mpin);
  
  //trash_clean(); 
  return 0;
}
