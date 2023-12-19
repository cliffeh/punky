#include "parse.h"
#include "punky.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  yyscan_t scanner;
  yylex_init (&scanner);
  // yyset_in (in, scanner);

  FILE *out = stdout;

  int rc;
  sexpr *result;
  // TODO pass in something to hang the result off of, and do something with it
  while ((rc = yyparse (&result, scanner)) == 0 && result)
    {
      result = eval (result);
      print (out, result);
    }

  yylex_destroy (scanner);

  return rc;
}
