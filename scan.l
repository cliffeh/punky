%{
#include "punky.h"
#include "parse.h"

#define YY_DECL int yylex \
  (                       \
   YYSTYPE *yylval_param, \
   YYLTYPE* yylloc_param, \
   yyscan_t yyscanner     \
  )

#define YY_USER_ACTION                        \
  yylloc->first_line = yylloc->last_line;     \
  yylloc->first_column = yylloc->last_column; \
  if (yylloc->last_line == yylineno)          \
    yylloc->last_column += yyleng;            \
  else {                                      \
    yylloc->last_line = yylineno;             \
    yylloc->last_column = yyleng;             \
  }

%}

%option noyywrap nounput noinput
%option reentrant bison-bridge bison-locations

%%

 /* stuff to ignore */
\n      { yylineno++; } // ignore newlines, but count them
[ \t]*  // ignore whitespace

 /* if we don't know what it is, just return it */
. return *yytext;

<<EOF>> return EOF;
