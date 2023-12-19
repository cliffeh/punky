%define api.pure full
%locations
%define parse.error verbose
%parse-param { sexpr **result }
%param       { void *scanner  }

%union {
  int num;
  char *str;
  sexpr *sexpr;
}

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

%type <sexpr> sexpr

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
  '(' ')' 
{
  $$ = calloc(1, sizeof(sexpr));
}
;

%%


void
yyerror (YYLTYPE* yyllocp, sexpr **result, yyscan_t scanner, const char *msg)
{
  fprintf(stderr, "[line %d, column %d]: %s\n",
          yyllocp->first_line, yyllocp->first_column, msg);
}
