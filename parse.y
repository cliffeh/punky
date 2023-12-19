%define api.pure full
%define api.value.type { sexpr * }
%locations
%define parse.error verbose
%parse-param { sexpr **result }
%param       { void  *scanner }

%code requires {
  #include "punky.h"
  #include <string.h>
  #include <stdio.h>
  #include <stdlib.h>
  typedef void* yyscan_t;
}

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

%token IDENT INTLIT STRLIT

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
  $$ = new_nil();
}
| IDENT
| INTLIT
| STRLIT
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
