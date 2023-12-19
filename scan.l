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

%x STRING
%%

 /* literals */
0|-?[1-9][0-9]*       { *yylval = new_int(atoi(yytext));  return(INTLIT); }

\" { // TODO this needs entirely more rigor
  BEGIN STRING;
}
<STRING>(\\\"|[^\"])* { *yylval = new_str(strdup(yytext)); return(STRLIT); }
<STRING>\" {
  BEGIN INITIAL;
}

 /* stuff to ignore */
\n      { yylineno++; } // ignore newlines, but count them
[ \t]*  // ignore whitespace
;.*     // ignore comments

 /* if we don't know what it is, just return it */
. return *yytext;

<<EOF>> return EOF;
