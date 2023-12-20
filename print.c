#include "punky.h"

void
sexpr_print (FILE *out, int flags, sexpr *e)
{
  switch (e->s_type)
    {
    case S_ERR:
      fprintf (out, "%s", e->sval);
      break;
    case S_NIL:
      fprintf (out, "nil"); // ()
      break;
    case S_INT:
      fprintf (out, "%d", e->ival);
      break;
    case S_STR:
      fprintf (out, "\"%s\"", e->sval);
      break;
    case S_QUOTE:
      fprintf (out, "(quote ");
      sexpr_print (out, flags + 1, e->car);
      fprintf (out, ")");
      break;
    case S_IDENT:
      fprintf (out, "%s", e->sval);
      break;
    case S_PAIR:
      fprintf (out, "(");
      sexpr_print (out, flags + 1, e->car);
      fprintf (out, " . ");
      sexpr_print (out, flags + 1, e->cdr);
      fprintf (out, ")");
      break;
    case S_LIST:
      fprintf (out, "(");
      sexpr_print (out, flags + 1, e->car);
      for (sexpr *cdr = e->cdr; cdr->s_type != S_NIL; cdr = cdr->cdr)
        {
          fprintf (out, " ");
          sexpr_print (out, flags + 1, cdr->car);
        }
      fprintf (out, ")");
      break;
    case S_BUILTIN:
      fprintf(out, "%s", e->sval);
      break;
    default:
      fprintf (stderr, "print: unknown expression type\n");
    }

  if ((flags & 0x00FFFFFF) == 0)
    fprintf (out, "\n");
}
