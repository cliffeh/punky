#include "parse.h"
#include "punky.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  FILE *in = stdin, *out = stdout;

  yyscan_t scanner;
  yylex_init (&scanner);
  yyset_in (in, scanner);

  int rc, flags = 0;
  environment env;
  sexpr *result;

  while ((rc = yyparse (&result, scanner)) == 0 && result)
    {
      result = sexpr_eval (&env, result);
      sexpr_print (out, flags, result);
      // free (result);
    }

  yylex_destroy (scanner);

  return rc;
}
