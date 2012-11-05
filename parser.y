%{
#include <stdio.h>
#include <string.h>
#include "punky.h"
#include "eval.h"
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
  char *strval;
  struct expr_t *e;
}

%token LPAREN RPAREN PLUS DASH STAR FSLASH SQUOTE DOT EOFTOK
 // variable/function definition
%token DEFINE
 // anonymous functions
%token LAMBDA
 // list operations
%token CAR CDR CONS LIST
 // program control/looping structures
%token IF WHILE
 // misc operations
%token LET QUOTE
 // string operations
%token SUBSTR
 // booleans and boolean operations
%token TRU FALS NOT AND OR
 // equality
%token EQUAL LT GT LE GE
 // atoms (do we need to define their types?)
%token <e> INTLIT FLOATLIT
%token <strval> IDENT STRLIT

 // non-terminals
%type <e> sexpr list seq atom op

%start program

%%

program: /* empty */
| EOFTOK { p->e = 0; YYACCEPT; }
| program sexpr { p->e = $2; YYACCEPT; }
;

sexpr : list
| atom
| SQUOTE sexpr { $$ = _list_expr(_op_expr(strdup("quote"), &eval_op_quote), _list_expr($2, &NIL)); }
;

atom: INTLIT { $$ = _int_expr(atoi(yytext)); } 
| FLOATLIT { $$ = _float_expr(atof(yytext)); }
| STRLIT { $$ = _str_expr(strdup(yytext)); }
| IDENT { $$ = _id_expr(strdup(yytext)); }
| TRU   { $$ = &T; }
| FALS  { $$ = &F; }
;

list: LPAREN seq RPAREN { $$ = $2; }
| LPAREN sexpr DOT sexpr RPAREN { $$ = _list_expr($2, $4); }
| LPAREN op seq RPAREN { $$ = _list_expr($2, $3); }
;

seq: /* empty */ { $$ = &NIL; }
| sexpr seq { $$ = _list_expr($1, $2); }
;

op: PLUS { $$ = _op_expr(strdup(yytext), &eval_op_add); }
| DASH   { $$ = _op_expr(strdup(yytext), &eval_op_sub); }
| STAR   { $$ = _op_expr(strdup(yytext), &eval_op_mul); }
| FSLASH { $$ = _op_expr(strdup(yytext), &eval_op_div); }
| DEFINE { $$ = _op_expr(strdup(yytext), &eval_op_define); }
| LAMBDA { $$ = _op_expr(strdup(yytext), &eval_op_lambda); }
| CAR    { $$ = _op_expr(strdup(yytext), &eval_op_car); }
| CDR    { $$ = _op_expr(strdup(yytext), &eval_op_cdr); }
| CONS   { $$ = _op_expr(strdup(yytext), &eval_op_cons); }
| LIST   { $$ = _op_expr(strdup(yytext), &eval_op_list); }
| QUOTE  { $$ = _op_expr(strdup(yytext), &eval_op_quote); }
| LET    { $$ = _op_expr(strdup(yytext), &eval_op_let); }
| IF     { $$ = _op_expr(strdup(yytext), &eval_op_if); }
| NOT    { $$ = _op_expr(strdup(yytext), &eval_op_not); }
| AND    { $$ = _op_expr(strdup(yytext), &eval_op_and); }
| OR     { $$ = _op_expr(strdup(yytext), &eval_op_or); }
| EQUAL  { $$ = _op_expr(strdup(yytext), &eval_op_equal); }
| LT     { $$ = _op_expr(strdup(yytext), &eval_op_lt); }
| GT     { $$ = _op_expr(strdup(yytext), &eval_op_gt); }
/*
| LE     { $$=LE_OP; }
| GE     { $$=GE_OP; }
| SUBSTR { $$=SUBSTR_OP; }
*/
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
	  "parse: unable to parse '%s'\n", yytext); 
}

yywrap()
{
  return(1);
}

