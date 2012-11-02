%{
#include "types.h"
#include "symbols.h" // symbol table
%}

%%

;.* /* ignore comments */

[\+\-]?[0-9]+ {
  return(INTLIT);
}

[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)? {
  return(FLOATLIT);
}

"#t" { return(TRU); }
"#f" { return(FALS); }

%{ /* boolean operations */ %}
"not" { return(NOT); }
"and" { return(AND); }
"or"  { return(OR); }

%{ /* variable/function definition */ %}
"defvar" { return(DEFVAR); }
"defun"  { return(DEFUN);  }

%{ /* list operations */ %}
"car"    { return(CAR); }
"cdr"    { return(CDR); }
"cons"   { return(CONS); }
"list"   { return(LIST); }

%{ /* program control/looping structures */ %}
"if"     { return(IF); }
"while"  { return(WHILE); }

%{ /* misc operations */ %}
"let"    { return(LET); }
"quote"  { return(QUOTE); }

%{ /* string operations */ %}
"substr" { return(SUBSTR); }

%{ /* equality operations */ %}
"="  { return(EQUAL); }
"<"  { return(LT); }
">"  { return(GT); }
"<=" { return(LE); }
">=" { return(GE); }

[_a-zA-Z][_\-a-zA-Z0-9]* {
  return(IDENT);
}

\"[^\"]*\" {
  // trim the quotes
  yytext++;
  yytext[strlen(yytext)-1] = '\0';
  return(STRLIT);
}

"(" { return(LPAREN); }
")" { return(RPAREN); }

"+" { return(PLUS); }
"-" { return(DASH); }
"*" { return(STAR); }
"/" { return(FSLASH); }

"'" { return(SQUOTE); }
"." { return(DOT); }

\n       /* ignore end of line */
[ \t]+   /* ignore whitespace */

<<EOF>> { return(EOFTOK); }

%%
// yywrap(){ return 1; }
