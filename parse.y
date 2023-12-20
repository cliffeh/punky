%define api.pure full
%define api.value.type { sexpr * }
%locations
%define parse.error verbose
%parse-param { sexpr **result }
%param       { void  *scanner }

%code requires {
  #include "alloc.h"
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

// literals & identifiers
%token BUILTIN IDENT INTLIT STRLIT

%start program

%%

program:
  sexpr
{
  *result = $1;
  YYACCEPT;
}
| error
{
  // *result set in yyerror
  YYACCEPT;
  // yyclearin;
  // yyerrok;
}
| YYEOF
{
  *result = 0;
}
;

sexpr: 
  atom
| '(' elements ')'
{ // list
  $$ = $2;
}
| '(' sexpr[car] '.' sexpr[cdr] ')'
{ // pair
  $$ = new_pair($car, $cdr);
}
| '\'' sexpr
{ // quote
  $$ = new_quote($2);
}
;

atom:
  '(' ')'
{
  $$ = new_nil();
}
| IDENT
| INTLIT
| STRLIT
| BUILTIN
;

elements:
  sexpr[car]
{
  $$ = new_list($car, new_nil());
}
| sexpr[car] elements[cdr]
{
  $$ = new_list($car, $cdr);
}
;

%%

void
yyerror (YYLTYPE* yyllocp, sexpr **result, yyscan_t scanner, const char *msg)
{
  // TODO get rid of this in favor of returning an error expression?
  // fprintf(stderr, "[line %d, column %d]: %s\n",
  //         yyllocp->first_line, yyllocp->first_column, msg);
  *result = new_err("[line %d, column %d]: %s\n",
          yyllocp->first_line, yyllocp->first_column, msg);
}
