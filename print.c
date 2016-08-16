#include "punky.h"

static void nl_and_spaces(FILE *out, int count)
{
  fprintf(out, "\n");
  int i;
  for(i = 0; i < count; i++) fputc(' ', out);
}

char *type_to_string(int t)
{
  switch(t) {
  case LIST_T: return "LIST";
  case BOOL_T: return "BOOL";
  case INT_T: return "INTEGER";
  case FLOAT_T: return "FLOAT";
  case STRING_T: return "STRING";
  case IDENT_T: return "IDENT";
  case OP_T: return "OP";
  case FUN_T: return "FUN";
  case PORT_T: return "PORT";
    
    /* special types */
  case NIL_T: return "NIL";
  case ERR_T: return "ERR";
  case EOF_T: return "<<EOF>>"; // this probably shouldn't happen
    
  default: return "UNKNOWN";
  }
}

/* static */ void _print(FILE *out, expr_t *e, int indent, int depth)
{
  switch(e->type){

  case LIST_T: {
    // is this the only case where we need to pretty-print something?
    if(indent && depth) nl_and_spaces(out, indent*depth); 
    fprintf(out, "(");
    _print(out, e->car, indent, depth+1);
    expr_t *ptr;
    for(ptr = e->cdr; ptr->type == LIST_T; ptr = ptr->cdr) {
      fprintf(out, " ");
      _print(out, ptr->car, indent, depth+1);
      //      if(ptr->cdr != &NIL) fprintf(out, " ");
    }
    if(ptr != &NIL) { // we must have a dot expression
      fprintf(out, " . ");
      _print(out, ptr, indent, depth+1);
    }
    fprintf(out, ")");
    if(indent && depth) nl_and_spaces(out, indent*(depth-1)); 
  }break;

  case BOOL_TRUE_T: fprintf(out, "#t"); break;
  case BOOL_FALSE_T: fprintf(out, "#f"); break;
  case INT_T: fprintf(out, "%i", e->intval); break;
  case FLOAT_T: fprintf(out, "%f", e->floatval); break;
  case STRING_T: fprintf(out, "\"%s\"", e->strval); break;
  case IDENT_T: fprintf(out, "%s", e->strval); break;

  case OP_T: {
    fprintf(out, "%s", e->strval);
  }break;

  case FUN_T: fprintf(out, "<function>"); break; // TODO make this prettier?

  case PORT_T: fprintf(out, "<port>"); break;
    
  case NIL_T: fprintf(out, "()"); break;
  case ERR_T: {
    for(expr_t *cdr = e; cdr != &NIL; cdr = cdr->cdr) {
      if(cdr && cdr->car && cdr->car->strval)
	fprintf(out, "\nerror: %s", cdr->car->strval);
    }
  }break;
  case EOF_T: fprintf(out, "<<EOF>>"); break; // this probably shouldn't happen

  case ENV_T: {
    _print(out, e->car, 0, 0);
  }
    
  default: fprintf(stderr, "print: error: unknown expression type: %s\n", type_to_string(e->type)); return;
  }

  // we're always going to want a newline after the initial print
  if(!depth) fprintf(out, "\n");
}

punky_t *punky_print(punky_t *p)
{
  // TODO error-checking of p->e?
  _print(p->out, p->e, (p->pretty) ? p->indent : 0, 0);
  return p;
}
