#include "punky.h"

static expr_t *_new_expr(enum PUNKY_TYPE type)
{
  expr_t *e = malloc(sizeof(expr_t));
  e->type = type;
  return e;
}

expr_t *_list_expr(expr_t *car, expr_t *cdr)
{
  expr_t *e = _new_expr(LIST_T);
  // e->type = LIST;
  e->car = car;
  e->cdr = cdr;
  return e;
}

expr_t *_int_expr(int value)
{
  expr_t *e = _new_expr(INTEGER_T);
  // e->type = INTEGER;
  e->intval = value;
  return e;
};

expr_t *_float_expr(float value)
{
  expr_t *e = _new_expr(FLOAT_T);
  // e->type = FLOAT;
  e->floatval = value;
  return e;
}

expr_t *_str_expr(char *value)
{
  expr_t *e = _new_expr(STRING_T);
  // e->type = STRING;
  e->strval = value;
  return e;
}

expr_t *_id_expr(char *value)
{
  expr_t *e = _new_expr(IDENTIFIER_T);
  // e->type = IDENTIFIER;
  e->strval = value;
  return e;
}

expr_t *_op_expr(enum PUNKY_OP_TYPE op)
{
  expr_t *e = _new_expr(OP_T);
  e->op = op;
  return e;
}

/* casting; would like to get rid of these... */
expr_t *_to_int(expr_t *e)
{
  switch(e->type) {
  case INTEGER_T: return e;
  case FLOAT_T: { expr_t *result = _int_expr(((int)e->floatval)); _free_expr(e); return result; }
  case ERROR_T: return e;
  default: return _error("attempt to convert non-numeric value to an integer");
  }
}

expr_t *_to_float(expr_t *e)
{
  switch(e->type) {
  case INTEGER_T: { expr_t *result = _float_expr(((float)e->intval)); _free_expr(e); return result; }
  case FLOAT_T: return e;
  case ERROR_T: return e;
  default: return _error("attempt to convert non-numeric value to a float");
  }
}

expr_t *_clone_expr(expr_t *e)
{
  switch(e->type) {
  case LIST_T: return _list_expr(_clone_expr(e->car), _clone_expr(e->cdr));
  case INTEGER_T: return _int_expr(e->intval);
  case FLOAT_T: return _float_expr(e->floatval);
  case STRING_T: return _str_expr(strdup(e->strval));
  case IDENTIFIER_T: return _id_expr(strdup(e->strval));
  case OP_T: return _op_expr(e->op);

  case NIL_T: return &NIL;
  case ERROR_T: return &ERROR;

  default: return _error("asked to copy an unknown type of expression");
  }
}

void _free_expr(expr_t *e)
{
  switch(e->type) {

  case LIST_T: {
    _free_expr(e->car);
    _free_expr(e->cdr);
  }break;
    
  case INTEGER_T: case FLOAT_T: { 
    // e will be freed at the end
  }break;
    
  case STRING_T: case IDENTIFIER_T: {
    free(e->strval);
  }break;

  case OP_T: {
    // nothing to do!
    // _free_expr(e->operands);
  }break;
    
  case ERROR_T: {
    // we won't free the expr_t itself, but we may as well let go of the string
    if(e->strval) { free(e->strval); e->strval = 0; }
    return;
  }break;
    
  case NIL_T: {
    // this probably shouldn't happen, but we'll allow it to go unnoticed...
    return;
  }break;
    
  default: {
    // oops!
    fprintf(stderr, "warning: attempt to free unrecognized expression type\n");
  }
  }
  free(e);
}
