#include "punky.h"
#include "eval.h"

static expr_t *_new_expr(int type)
{
  expr_t *e = calloc(1, sizeof(expr_t));
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
  expr_t *e = _new_expr(INT_T);
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
  expr_t *e = _new_expr(IDENT_T);
  e->strval = value;
  e->eval = &eval_ident;
  return e;
}

expr_t *_op_expr(char *name, expr_t * (*eval)(struct expr_t *, const struct expr_t *))
{
  expr_t *e = _new_expr(OP_T);
  e->strval = name;
  e->eval = eval;
  return e;
}

expr_t *_fun_expr(expr_t *formals, expr_t *body)
{
  expr_t *e = _new_expr(FUN_T);
  e->car = formals;
  e->cdr = body;
  e->eval = &eval_clone;
  return e;
}

expr_t *_port_expr(FILE *fp)
{
  expr_t *e = _new_expr(PORT_T);
  e->fp = fp;
  return e;
}

expr_t *__re_expr(regex_t *re)
{
  expr_t *e = _new_expr(RE_T);
  e->re = re;
  e->eval = &eval_clone;
  return e;
}

expr_t *_re_expr(char *re)
{
  regex_t *compiled = calloc(1, sizeof(regex_t));
  regcomp(compiled, re+1, 0); // TODO support flags
  return __re_expr(compiled);
}

expr_t *_err_expr(expr_t *cdr, const char *msg, const char *opt)
{
  expr_t *e = _new_expr(ERR_T);
  char *str;
  if(opt) {
    str = calloc(strlen(msg) + strlen(opt) + 5, sizeof(char)); // colon, space, quotes, null term
    sprintf(str, "%s: '%s'", msg, opt);
  } else {
    str = strdup(msg);
  }

  e->car = _str_expr(str);
  // TODO do something intelligent with exprs that aren't errors?
  e->cdr = (cdr && (cdr->type == ERR_T)) ? cdr : &NIL;
  return e;
}

expr_t *_env_expr(expr_t *values, expr_t *parent)
{
  expr_t *e = _new_expr(ENV_T);
  e->car = values;
  e->cdr = parent;
  return e;
}

expr_t *_clone_expr(const expr_t *e)
{
  switch(e->type) {
  case LIST_T: return _list_expr(_clone_expr(e->car), _clone_expr(e->cdr));
  case BOOL_TRUE_T: return &T;
  case BOOL_FALSE_T: return &F;
  case INT_T: return _int_expr(e->intval);
  case FLOAT_T: return _float_expr(e->floatval);
  case STRING_T: return _str_expr(strdup(e->strval));
  case IDENT_T: return _id_expr(strdup(e->strval));
  case OP_T: return _op_expr(strdup(e->strval), e->eval);
  case FUN_T: return _fun_expr(_clone_expr(e->car), _clone_expr(e->cdr));
  case PORT_T: return _port_expr(e->fp);
  case ENV_T: return _env_expr(_clone_expr(e->car), &NIL); // TODO clone parent?
  case RE_T: return __re_expr(memcpy(calloc(1, sizeof(regex_t)), e->re, 1));

  case EOF_T: return &_EOF;
  case NIL_T: return &NIL;

  default: {
    return _err_expr(0, "internal: attempt to clone an unknown expression type", 0);
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
  case INT_T: {
    if(e1->intval == e2->intval) return 0;
    return (e1->intval > e2->intval) ? 1 : -1;
  }
  case FLOAT_T: {
    if(e1->floatval == e2->floatval) return 0;
    return (e1->floatval > e2->floatval) ? 1 : -1;
  }
  case STRING_T: return strcmp(e1->strval, e2->strval);
  case PORT_T: return e1->fp == e2->fp;
  case RE_T: return memcmp(e1->re, e2->re, sizeof(regex_t));
  default: return (e1 == e2) ? 0 : -1;
  }
}

void _free_expr(expr_t *e)
{
  switch(e->type) {

  case LIST_T: case FUN_T: case ERR_T: {
    _free_expr(e->car);
    _free_expr(e->cdr);
  }break;
    
  case INT_T: case FLOAT_T: case PORT_T: {
    // e will be freed at the end
  }break;
    
  case STRING_T: case IDENT_T: {
    free(e->strval);
  }break;

  case OP_T: {
    // nothing to do!
    // _free_expr(e->operands);
    free(e->strval);
  }break;
    
  case BOOL_TRUE_T: case BOOL_FALSE_T: case NIL_T: case EOF_T: {
    return;
  }break;

  case ENV_T: {
    _free_expr(e->car);
    // leave the parent alone, let it be explicitly reaped
  }break;

  case RE_T: {
    free(e->re);
  }break;
    
  default: {
    // oops!
    fprintf(stderr, "warning: attempt to free unrecognized expression type\n");
  }
  }
  free(e);
}
