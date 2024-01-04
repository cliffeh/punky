#include "env.h"
#include "eval.h"
#include "parse.h"
#include "print.h"
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
  env_init (&env, 0);
  sexpr *result = 0;

  while ((rc = yyparse (&result, scanner)) == 0 && result)
    {
      sexpr *e = result->eval (result, &env); // sexpr_eval (&env, result);
      sexpr_free (result);
      sexpr_print (out, flags, e);
      sexpr_free (e);
    }

  env_destroy (&env);
  yylex_destroy (scanner);

  return rc;
}
