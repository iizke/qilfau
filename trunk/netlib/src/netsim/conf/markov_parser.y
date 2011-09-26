
%{
//#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "vexpr.h"
#include "error.h"
//long debug = 0;
//CONFIG conf;
static VEXPR *vexpr = NULL;
VEXPR_LIST vexpr_list;
%}

%locations
%pure_parser
 
%union {char* str; double val;}

%token EQ
%token OR
%token EQUAL
%token ENDLINE
%token <str> VAR
%token PP
%token IF
%token THEN
%token ANDO
%token <val> NUM
%token <str> STRING
/* %type <val> exp */
%type <str> vexp
/* %type <str> poison */

%left '?'
%left ANDO
%left EQ
%left '<' '>' EQUAL OR 
%left '+' '-'
%left '*' '/'
%% /* Grammar rules and actions follow */
input:    /* empty */
        | input line 
;

line:		vexp ENDLINE {  
                if (!vexpr) try( vexpr_new(&vexpr) );
                vexpr_assign_value(vexpr, $1); 
                vexpr_list_insert (&vexpr_list, vexpr);
                vexpr = NULL; 
            }
        |   ENDLINE
        
;

vexp:   VAR {if (!vexpr) try(vexpr_new (&vexpr)); $$ = vexpr_declare_varid(vexpr, vexpr_str2id($1), 1);}
    |   NUM {$$ = vexpr_node_const($1);}
    |   vexp '+' vexp {$$ = vexpr_node_formular(VEXPR_OP_PLUS, $1, $3);}
    |   vexp '-' vexp {$$ = vexpr_node_formular(VEXPR_OP_MINUS, $1, $3);}
    |   vexp '*' vexp {$$ = vexpr_node_formular(VEXPR_OP_MUL, $1, $3);}
    |   vexp '/' vexp {$$ = vexpr_node_formular(VEXPR_OP_DIV, $1, $3);}
    |   vexp '>' vexp {$$ = vexpr_node_formular(VEXPR_OP_GT, $1, $3);}
    |   vexp '<' vexp {$$ = vexpr_node_formular(VEXPR_OP_LT, $1, $3);}
    |   vexp OR vexp {$$ = vexpr_node_formular(VEXPR_OP_OR, $1, $3);}
    |   vexp EQUAL vexp {$$ = vexpr_node_formular(VEXPR_OP_EQUAL, $1, $3);}
    |   '(' vexp ')' {$$ = $2;}
    |   vexp '?' vexp {$$ = vexpr_node_formular(VEXPR_OP_INFER, $1, $3);}
    |   vexp ANDO vexp {$$ = vexpr_node_formular(VEXPR_OP_ANDO, $1, $3);}
    |   vexp EQ vexp {$$ = vexpr_node_formular(VEXPR_OP_ASSIGN, $1, $3);}
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

int main_markov_parser (int argc, char** argv) {
  extern FILE *mpin; 
  extern int mplex();
  extern int mpparse();
  VEXPR_LIST *v = NULL;
  //trash_init();
  vexpr_list_init(&vexpr_list);
  
  mpin = fopen("/home/iizke/projects/netlib/src/netsim/conf/calc.txt", "r");
  mpparse();
  fclose(mpin);
  
  //vexpr_list_calc_1 (&vexpr_list, 's', 10);
  v = vexpr_list.next;
  while (v != &vexpr_list) {
    VEXPR *vexpr = vexpr_from_linked_list(v);
    vexpr_view_vars (vexpr);
    
    v = v->next;
  }
  
  //trash_clean(); 
  return 0;
}
