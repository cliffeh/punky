#include "punky.h"

#include <stdio.h>

sexpr *
eval (sexpr *e, environment *env)
{
  switch (e->type)
    {
    case SEXPR_INT:
    case SEXPR_NIL:
    case SEXPR_STR:
      return e;
    default:
      fprintf (stderr, "eval: unknown expression type\n");
    }
}

void
print (FILE *out, sexpr *e)
{
  switch (e->type)
    {
    case SEXPR_INT:
      fprintf(out, "%d\n", e->ival);
      break;
    case SEXPR_NIL:
      fprintf (out, "nil\n"); // ()
      break;
    case SEXPR_STR:
      fprintf(out, "\"%s\"\n", e->sval);
      break;
    default:
      fprintf (stderr, "print: unknown expression type\n");
    }
}
