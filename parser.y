%{
#include <stdio.h>
#include <string.h>
#include "punky.h"
#include "eval.h"
  extern char *yytext;
  extern FILE *yyin; // , *yyout;
  int yylex();
  int yyerror();
  %}

// %define api.pure
%pure-parser

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
%token CAR CDR CONS LIST APPEND
 // program control/looping structures
%token IF
 // misc operations
%token LET QUOTE
 // string operations
%token SUBSTR STRLEN SPLIT CONCAT
 // booleans and boolean operations
%token TRU FALS NOT AND OR
 // equality
%token EQUAL LT GT LE GE
 // ports
%token OPENIF CLOSEIF READLINE
 // environments
%token ENV KEYS
 // atoms (do we need to define their types?)
%token <e> INTLIT FLOATLIT
%token <strval> IDENT STRLIT

 // non-terminals
%type <e> sexpr list seq atom op

%start program

%%

program: /* empty */
| EOFTOK { p->e = &_EOF; YYACCEPT; }
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

op: PLUS   { $$ = _op_expr(strdup(yytext), &eval_op_add); }
| DASH     { $$ = _op_expr(strdup(yytext), &eval_op_sub); }
| STAR     { $$ = _op_expr(strdup(yytext), &eval_op_mul); }
| FSLASH   { $$ = _op_expr(strdup(yytext), &eval_op_div); }
| DEFINE   { $$ = _op_expr(strdup(yytext), &eval_op_define); }
| LAMBDA   { $$ = _op_expr(strdup(yytext), &eval_op_lambda); }
| CAR      { $$ = _op_expr(strdup(yytext), &eval_op_car); }
| CDR      { $$ = _op_expr(strdup(yytext), &eval_op_cdr); }
| CONS     { $$ = _op_expr(strdup(yytext), &eval_op_cons); }
| LIST     { $$ = _op_expr(strdup(yytext), &eval_op_list); }
| APPEND   { $$ = _op_expr(strdup(yytext), &eval_op_append); }
| QUOTE    { $$ = _op_expr(strdup(yytext), &eval_op_quote); }
| LET      { $$ = _op_expr(strdup(yytext), &eval_op_let); }
| IF       { $$ = _op_expr(strdup(yytext), &eval_op_if); }
| NOT      { $$ = _op_expr(strdup(yytext), &eval_op_not); }
| AND      { $$ = _op_expr(strdup(yytext), &eval_op_and); }
| OR       { $$ = _op_expr(strdup(yytext), &eval_op_or); }
| EQUAL    { $$ = _op_expr(strdup(yytext), &eval_op_equal); }
| LT       { $$ = _op_expr(strdup(yytext), &eval_op_lt); }
| GT       { $$ = _op_expr(strdup(yytext), &eval_op_gt); }
| LE       { $$ = _op_expr(strdup(yytext), &eval_op_le); }
| GE       { $$ = _op_expr(strdup(yytext), &eval_op_ge); }
| SUBSTR   { $$ = _op_expr(strdup(yytext), &eval_op_substr); }
| STRLEN   { $$ = _op_expr(strdup(yytext), &eval_op_strlen); }
| CONCAT   { $$ = _op_expr(strdup(yytext), &eval_op_concat); }
| SPLIT    { $$ = _op_expr(strdup(yytext), &eval_op_split); }
| OPENIF   { $$ = _op_expr(strdup(yytext), &eval_op_openif); }
| CLOSEIF  { $$ = _op_expr(strdup(yytext), &eval_op_closeif); }
| READLINE { $$ = _op_expr(strdup(yytext), &eval_op_readline); }
| ENV      { $$ = _op_expr(strdup(yytext), &eval_op_env); }
| KEYS     { $$ = _op_expr(strdup(yytext), &eval_op_keys); }
;

%%
int yyerror(s)
char *s;
{
  return 1;
}

punky_t *punky_read(punky_t *p)
{
  switch(yyparse(p)) {
  case 0: return (p->e != &_EOF) ? p : 0;
  case 1: p->e = _err_expr(0, "read: unable to parse", yytext); return p;
  case 2: p->e = _err_expr(0, "read: memory exhaustion", 0); return p;
  }

  // this shouldn't happen, but if it does...
  return 0;
}

int yywrap()
{
  return(1);
}
