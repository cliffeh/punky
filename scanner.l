%{
#include "types.h"
#include "punky.h"
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
"define" { return(DEFINE); }
"def"    { return(DEFINE); } // shorthand

%{ /* anonymous functions */ %}
"lambda" { return(LAMBDA); }

%{ /* list operations */ %}
"car"    { return(CAR); }
"cdr"    { return(CDR); }
"cons"   { return(CONS); }
"list"   { return(LIST); }
"append" { return(APPEND); }

%{ /* program control/looping structures */ %}
"if"     { return(IF); }

%{ /* misc operations */ %}
"let"    { return(LET); }
"quote"  { return(QUOTE); }

%{ /* string operations */ %}
"substr" { return(SUBSTR); }
"strlen" { return(STRLEN); }
"split"  { return(SPLIT);  }
"concat" { return(CONCAT); }

%{ /* equality operations */ %}
"="  { return(EQUAL); }
"<"  { return(LT); }
">"  { return(GT); }
"<=" { return(LE); }
">=" { return(GE); }

%{ /* port operations */ %}
"open-input-file"  { return(OPENIF);   }
"close-input-file" { return(CLOSEIF);  }
"readline"         { return(READLINE); }

%{ /* env operations */ %}
"env"  { return(ENV); }
"keys" { return(KEYS); }

[_a-zA-Z][\?_\-a-zA-Z0-9]* {
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
