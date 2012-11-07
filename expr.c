#include "punky.h"
#include "eval.h"

static expr_t *_new_expr(enum PUNKY_TYPE type)
{
  expr_t *e = malloc(sizeof(expr_t));
  e->type = type;
  return e;
}

expr_t *_list_expr(expr_t *car, expr_t *cdr)
{
  expr_t *e = _new_expr(LIST_T);
  e->car = car;
  e->cdr = cdr;
  e->eval = &eval_list;
  return e;
}

expr_t *_int_expr(int value)
{
  expr_t *e = _new_expr(INTEGER_T);
  e->intval = value;
  e->eval = &eval_clone;
  return e;
};

expr_t *_float_expr(float value)
{
  expr_t *e = _new_expr(FLOAT_T);
  e->floatval = value;
  e->eval = &eval_clone;
  return e;
}

expr_t *_str_expr(char *value)
{
  expr_t *e = _new_expr(STRING_T);
  e->strval = value;
  e->eval = &eval_clone;
  return e;
}

expr_t *_id_expr(char *value)
{
  expr_t *e = _new_expr(IDENTIFIER_T);
  e->strval = value;
  e->eval = &eval_ident;
  return e;
}

expr_t *_op_expr(char *name, expr_t * (*eval)(struct env_t *, struct expr_t *))
{
  expr_t *e = _new_expr(OP_T);
  e->strval = name;
  e->eval = eval;
  return e;
}

expr_t *_clone_expr(expr_t *e)
{
  switch(e->type) {
  case LIST_T: return _list_expr(_clone_expr(e->car), _clone_expr(e->cdr));
  case BOOL_T: return e;
  case INTEGER_T: return _int_expr(e->intval);
  case FLOAT_T: return _float_expr(e->floatval);
  case STRING_T: return _str_expr(strdup(e->strval));
  case IDENTIFIER_T: return _id_expr(strdup(e->strval));
  case OP_T: return _op_expr(strdup(e->strval), e->eval);

  case EOF_T: return &_EOF;
  case NIL_T: return &NIL;

  default: {
    fprintf(stderr, "error: attempt to clone an unknown expression type\n");
    return 0;
  }
  }
}

int compare(expr_t *e1, expr_t *e2)
{
  int comp;
  if(e1->type != e2->type) return -1;
  switch(e1->type) {
  case LIST_T: {
    return ((comp = compare(e1->car, e2->car)) == 0) ? compare(e1->cdr, e2->cdr) : comp;
  }
  case INTEGER_T: {
    if(e1->intval == e2->intval) return 0;
    return (e1->intval > e2->intval) ? 1 : -1;
  }
  case FLOAT_T: {
    if(e1->floatval == e2->floatval) return 0;
    return (e1->floatval > e2->floatval) ? 1 : -1;
  }
  case STRING_T: return strcmp(e1->strval, e2->strval);
  default: return (e1 == e2) ? 0 : -1;
  }
}

void _free_expr(expr_t *e, expr_t *not)
{
  if(e == not) return;

  switch(e->type) {

  case LIST_T: {
    _free_expr(e->car, not);
    _free_expr(e->cdr, not);
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
    free(e->strval);
  }break;
    
  case BOOL_T: case NIL_T: case EOF_T: {
    return;
  }break;
    
  default: {
    // oops!
    fprintf(stderr, "warning: attempt to free unrecognized expression type\n");
  }
  }
  free(e);
}
