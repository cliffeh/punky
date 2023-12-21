#include "parse.h"
#include "punky.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  FILE *in = stdin, *out = stdout;
  // TODO popt!
  if (argc == 2)
    {
      in = fopen (argv[1], "r");
    }

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
      sexpr_free (result);
    }

  yylex_destroy (scanner);

  return rc;
}
