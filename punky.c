#include "punky.h"

#include <stdio.h>
#include <stdlib.h>

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
print (FILE *out, int flags, sexpr *e)
{
  switch (e->type)
    {
    case SEXPR_INT:
      fprintf (out, "%d\n", e->ival);
      break;
    case SEXPR_NIL:
      fprintf (out, "nil\n"); // ()
      break;
    case SEXPR_STR:
      fprintf (out, "\"%s\"\n", e->sval);
      break;
    default:
      fprintf (stderr, "print: unknown expression type\n");
    }
}

sexpr *
new_nil ()
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_NIL;
  return e;
}

sexpr *
new_int (int ival)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_INT;
  e->ival = ival;
  return e;
}

sexpr *
new_str (char *sval)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_STR;
  e->sval = sval;
  return e;
}

sexpr *new_ident(char *sval)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_IDENT;
  e->sval = sval;
  return e;
}

sexpr *new_list(sexpr *car, sexpr *cdr)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_LIST;
  e->car = car;
  e->cdr = cdr;
  return e;
}
