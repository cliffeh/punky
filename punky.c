#include "punky.h"

#include <stdio.h>

sexpr *
eval (sexpr *e)
{
  switch (e->type)
    {
    case S_INT:
    case S_NIL:
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
    case S_INT:
      fprintf(out, "%d\n", e->ival);
      break;
    case S_NIL:
      fprintf (out, "nil\n"); // ()
      break;
    default:
      fprintf (stderr, "print: unknown expression type\n");
    }
}
