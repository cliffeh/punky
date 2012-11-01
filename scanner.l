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

"defvar" { return(DEFVAR); }
"defun"  { return(DEFUN);  }
"let"    { return(LET); }
"car"    { return(CAR); }
"cdr"    { return(CDR); }
"quote"  { return(QUOTE); }

[_a-zA-Z][_a-zA-Z0-9]* {
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

\'  { return(SQUOTE); }

\n       /* ignore end of line */
[ \t]+   /* ignore whitespace */

<<EOF>> { return(EOFTOK); }

%%
// yywrap(){ return 1; }
