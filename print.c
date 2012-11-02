#include "punky.h"

static void nl_and_spaces(FILE *out, int count)
{
  fprintf(out, "\n");
  int i;
  for(i = 0; i < count; i++) fputc(' ', out);
}

static void _print(FILE *out, expr_t *e, int indent, int depth)
{
  switch(e->type){

  case LIST_T: {
    // is this the only case where we need to pretty-print something?
    if(indent && depth) nl_and_spaces(out, indent*depth); 
    fprintf(out, "(");
    // _print(out, e->car, indent, depth+1);
    expr_t *ptr;
    for(ptr = e; ptr != &NIL; ptr = ptr->cdr) {
      //      fprintf(out, " ");
      _print(out, ptr->car, indent, depth+1);
      if(ptr->cdr != &NIL) fprintf(out, " ");
    }
    fprintf(out, ")");
    if(indent && depth) nl_and_spaces(out, indent*(depth-1)); 
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
    case SUBSTR_OP: fprintf(out, "substr"); break;
    default: fprintf(out, "UNKNOWN_OP");
    }
    // TODO COMPLETE THIS!
  }break;

  case NIL_T: fprintf(out, "()"); break;
  case ERROR_T: fprintf(stderr, "error: %s\n", e->strval); return;

  default: fprintf(stderr, "print error: unknown expression type %i\n", e->type); return;
  }

  // we're always going to want a newline after the initial print
  if(!depth) fprintf(out, "\n");
}

punky_t *print(punky_t *p)
{
  // TODO error-checking of p->e?
  _print(p->out, p->e, (p->pretty) ? p->indent : 0, 0);
  return p;
}
