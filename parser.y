%{
#include <stdio.h>
#include <string.h>
#include "punky.h"
  extern char *yytext;
  extern FILE *yyin; // , *yyout;
  %}

%define api.pure

// %parse-param { expr_t *e }
%parse-param { punky_t *p }
// %parse-param { FILE *out }
%initial-action
{
  yyin = p->in;
  // yyout = out;
}

%union {
  enum PUNKY_OP_TYPE op;
  char *strval;
  struct expr_t *e;
}

%token LPAREN RPAREN PLUS DASH STAR FSLASH SQUOTE DOT EOFTOK
 // variable/function definition
%token DEFVAR DEFUN 
 // list operations
%token CAR CDR CONS
 // misc operations
%token LET QUOTE
 // string operations
%token SUBSTR

 // atoms
%token <e> INTLIT FLOATLIT
%token <strval> IDENT STRLIT

 // non-terminals
%type <e> sexpr list seq atom
%type <op> op

%start program

%%

program: /* empty */
| EOFTOK { p->e = 0; YYACCEPT; }
| program sexpr { p->e = $2; YYACCEPT; }
;

sexpr : list
| atom
| SQUOTE sexpr { $$ = _list_expr(_op_expr(QUOTE_OP), _list_expr($2, &NIL)); }
;

atom: INTLIT { $$ = _int_expr(atoi(yytext)); } 
| FLOATLIT { $$ = _float_expr(atof(yytext)); }
| STRLIT { $$ = _str_expr(strdup(yytext)); }
| IDENT { $$ = _id_expr(strdup(yytext)); }
;

list: LPAREN seq RPAREN { $$ = $2; }
| LPAREN sexpr DOT sexpr RPAREN { $$ = _list_expr($2, $4); }
| LPAREN op seq RPAREN { $$ = _list_expr(_op_expr($2), $3); }
;

seq: /* empty */ { $$ = &NIL; }
| sexpr seq { $$ = _list_expr($1, $2); }
;

op: PLUS { $$=ADD_OP; } 
| DASH   { $$=SUB_OP; } 
| STAR   { $$=MUL_OP; } 
| FSLASH { $$=DIV_OP; }
| CAR    { $$=CAR_OP; } 
| CDR    { $$=CDR_OP; }
| CONS   { $$=CONS_OP; }
| QUOTE  { $$=QUOTE_OP; }
| DEFVAR { $$=DEFVAR_OP; }
| DEFUN  { $$=DEFUN_OP; }
| LET    { $$=LET_OP; }
| SUBSTR { $$=SUBSTR_OP; }
;

%%

punky_t *parse(punky_t *p)
{
  // clean up any leftover cruft
  if(p->e) _free_expr(p->e);
  yyparse(p); // TODO grab the return value?
  // p->e = _parse();
  return (p->e) ? p : 0;
}

yyerror(s)
char *s;
{
  // TODO handle parse errors better!
  /* ignore unknown input */ 
  fprintf(stderr,
	     "parse: unable to parse '%s'; ignoring this line\n", yytext); 
}

yywrap()
{
  return(1);
}

