%define api.pure full
%define api.value.type { sexpr * }
%locations
%define parse.error verbose
%parse-param { sexpr **result }
%param       { void *scanner  }

%code requires {
  #include "punky.h"
  #include <string.h>
  #include <stdio.h>
  #include <stdlib.h>
  typedef void* yyscan_t;
}

// %union {
//   int ival;
//   char *sval;
//   sexpr *sexpr;
// }

%code provides {
  // need all of these to prevent compiler warnings
  int  yylex_init (yyscan_t *scanner);
  void yyset_in (FILE *in, yyscan_t scanner);
  int  yylex_destroy (yyscan_t scanner);
}

%code {
  int yylex(YYSTYPE *yylvalp, YYLTYPE* yyllocp, yyscan_t scanner);
  void yyerror (YYLTYPE* yyllocp, sexpr **result, yyscan_t scanner, const char *msg);
}

%token INTLIT STRLIT

// %type <ival> INTLIT
// %type <sval> STRLIT
// %type <sexpr> atom elements list sexpr

%start program

%%

program:
  sexpr
{
  *result = $1;
  YYACCEPT;
}
| YYEOF
{
  *result = 0;
}
;

sexpr: 
  atom
| list
;

atom:
  '(' ')'
{
  $$ = calloc(1, sizeof(sexpr)); // nil
}
| INTLIT
{
  // $$ = calloc(1, sizeof(sexpr));
  // $$->type = S_INT;
  // $$->ival = $1;
}
| STRLIT
{
  // $$ = calloc(1, sizeof(sexpr));
  // $$->type = S_STR;
  // $$->sval = $1;
}
;

list:
  '(' elements ')'
{
  $$ = $2;
}
| '(' sexpr[car] '.' sexpr[cdr] ')'
{
  $$ = calloc(1, sizeof(sexpr));
  $$->type = SEXPR_LIST;
  $$->car = $car;
  $$->cdr = $cdr;
}
;

elements:
  sexpr[car]
{
  $$ = calloc(1, sizeof(sexpr));
  $$->type = SEXPR_LIST;
  $$->car = $car;
  $$->cdr = calloc(1, sizeof(sexpr)); // nil
}
| sexpr[car] elements[cdr]
{
  $$ = calloc(1, sizeof(sexpr));
  $$->type = SEXPR_LIST;
  $$->car = $car;
  $$->cdr = $cdr;
}
;

%%

void
yyerror (YYLTYPE* yyllocp, sexpr **result, yyscan_t scanner, const char *msg)
{
  fprintf(stderr, "[line %d, column %d]: %s\n",
          yyllocp->first_line, yyllocp->first_column, msg);
}
