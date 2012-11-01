#include "punky.h"

/* output flags */
#define NOFLAGS 0
#define PARENS  (1<<0)
#define NEWLINE (1<<1)
#define ROOT    (PARENS|NEWLINE)

static void __print(FILE *out, expr_t *e, int flags, int indent)
{
  // this definitely shouldn't happen...
  if(!e) { fprintf(stderr, "SHIT MANG WE GOT A NULL\n"); return; }

  switch(e->type){

  case LIST_T: {
    fprintf(out, "(");
    __print(out, e->car, flags, indent);
    expr_t *ptr;
    for(ptr = e->cdr; ptr != &NIL; ptr = ptr->cdr) {
      fprintf(out, " ");
      __print(out, ptr->car, flags, indent);
    }
    fprintf(out, ")");
  }break;

  case INTEGER_T: fprintf(out, "%i", e->intval); break;
  case FLOAT_T: fprintf(out, "%f", e->floatval); break;
  case STRING_T: fprintf(out, "\"%s\"", e->strval); break;
  case IDENTIFIER_T: fprintf(out, "%s", e->strval); break;

  case OP_T: {
    switch(e->op) {
    case ADD_OP: fprintf(out, "+"); break;
    case SUB_OP: fprintf(out, "-"); break;
    case MUL_OP: fprintf(out, "*"); break;
    case DIV_OP: fprintf(out, "/"); break;
    case CAR_OP: fprintf(out, "car"); break;
    case CDR_OP: fprintf(out, "cdr"); break;
    case QUOTE_OP: fprintf(out, "quote"); break;
    case DEFVAR_OP: fprintf(out, "defvar"); break;
    case DEFUN_OP: fprintf(out, "defun"); break;
    case LET_OP: fprintf(out, "let"); break;
    default: fprintf(out, "UNKNOWN_OP");
    }
    // TODO COMPLETE THIS!
  }break;

  case NIL_T: fprintf(out, "()"); break;
  case ERROR_T: fprintf(stderr, "error: %s\n", e->strval); return;

  default: fprintf(stderr, "print error: unknown expression type %i\n", e->type); return;
  }

  // if(flags&NEWLINE) fprintf(out, "\n");
}

static void _print(FILE *out, expr_t *e)
{
  __print(out, e, ROOT, 0);
  fprintf(out, "\n");
}

punky_t *print(punky_t *p)
{
  // TODO error-checking of p->e?
  _print(p->out, p->e);
  return p;
}
