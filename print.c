#include "builtin.h"
#include "punky.h"

void
sexpr_print (FILE *out, int flags, const sexpr *e)
{
  switch (e->s_type)
    {
    case S_ERR:
      fprintf (out, "%s", e->sval);
      // print the rest of the stack trace
      for (const sexpr *err = e->cdr; err && err->s_type == S_ERR; err = err->cdr)
        fprintf (out, "\n%s", err->sval);
      break;
    case S_NIL:
      fprintf (out, "()");
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
    case S_FUN:
      fprintf (out, "(lambda "); // TODO is this correct?
      sexpr_print (out, flags + 1, e->car);
      fprintf (out, " ");
      sexpr_print (out, flags + 1, e->cdr);
      fprintf (out, ")");
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
      for (sexpr *cdr = e->cdr; cdr != &NIL; cdr = cdr->cdr)
        {
          fprintf (out, " ");
          sexpr_print (out, flags + 1, cdr->car);
        }
      fprintf (out, ")");
      break;
    case S_BUILTIN:
      fprintf (out, "%s", e->sval);
      break;
    default:
      fprintf (stderr, "print: unknown expression type %d\n", e->s_type);
    }

  if ((flags & 0x00FFFFFF) == 0)
    fprintf (out, "\n");
}
