#include <errno.h>
#include "punky.h"
#include "types.h"
#include "env.h"
#include "eval.h"

/* static convenience functions */
static expr_t *eval_op_add_float(env_t *env, const expr_t *e, float partial);
static expr_t *eval_op_add_int(env_t *env, const expr_t *e, int partial);
static expr_t *eval_op_sub_float(env_t *env, const expr_t *e, float partial);
static expr_t *eval_op_sub_int(env_t *env, const expr_t *e, int partial);
static expr_t *eval_op_mul_float(env_t *env, const expr_t *e, float partial);
static expr_t *eval_op_mul_int(env_t *env, const expr_t *e, int partial);
static expr_t *eval_op_div_float(env_t *env, const expr_t *e, float partial);
static expr_t *eval_op_div_int(env_t *env, const expr_t *e, int partial);
static expr_t *eval_function_call(env_t *env, expr_t *formals, expr_t *body, expr_t *args);

expr_t *eval_clone(env_t *env, const expr_t *e)
{
  return _clone_expr(e);
}

expr_t *eval_op_define(env_t *env, const expr_t *e)
{
  expr_t *id = e->car;
  if(!IS_IDENT(id)) {
    return _err_expr(id, "eval: define: first argument must be an identifier", 0);
  }
  
  expr_t *value = e->cdr->car->eval(env, e->cdr->car), *r;
  if(!IS_ERR(value)) {
    put(env, id, value);
    _free_expr(value);
    r = _clone_expr(id);
  } else {
    r = _err_expr(value, "eval: define: second argument evaluated to an error", 0);
  }

  return r;
}

expr_t *eval_op_lambda(env_t *env, const expr_t *e)
{
  return _fun_expr(_clone_expr(e->car), _clone_expr(e->cdr));
}

static expr_t *eval_function_call(env_t *env, expr_t *formals, expr_t *body, expr_t *args)
{
  expr_t *f_ptr = formals, *a_ptr = args, *tmp, *r;

  env_t funenv;
  init_env(&funenv, env);
  
  // evaluate each arg and bind it to its formal parameter in the new env we've created
  while((f_ptr != &NIL) && (a_ptr != &NIL)) {
    expr_t *val = a_ptr->car->eval(env, a_ptr->car) ;
    put(&funenv, f_ptr->car, val);
    _free_expr(val);

    tmp = f_ptr->cdr;
    f_ptr = tmp;

    tmp = a_ptr->cdr;
    a_ptr = tmp;
  }

  // make sure we've provided the right number of arguments
  if((f_ptr != &NIL) || (a_ptr != &NIL)) {
    r = _err_expr(0, "eval: function: incorrect number of arguments to function", 0);
  } else {
    r = body->eval(&funenv, body);
  }

  // clean up and return
  free_env(&funenv);
  return r;
}

expr_t *eval_list(env_t *env, const expr_t *e)
{
  expr_t *r;
  if(IS_OP(e->car)) {
    // we already know how to execute ops
    r = e->car->eval(env, e->cdr);
  } else {
    expr_t *fun = e->car->eval(env, e->car);
    if(!IS_FUN(fun)) {
      r = _err_expr(fun, "eval: list: neither an operation nor a function", 0);
    } else {
      r = eval_function_call(env, fun->car, fun->cdr->car, e->cdr);
      _free_expr(fun);
    }
  }

  return r;
}

expr_t *eval_ident(env_t *env, const expr_t *e)
{
  return get(env, e);
}

static expr_t *eval_op_add_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: addfloat: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_add_float(env, e->cdr, partial + (float)e1->intval); break;
  case FLOAT_T: r = eval_op_add_float(env, e->cdr, partial + e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: addfloat: attempt to add a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_add_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    return _err_expr(0, "eval: addint: unexpected argument type", 0);
  }

  // we'll use this to hold the result
  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_add_int(env, e->cdr, partial + e1->intval); break;
  case FLOAT_T: r = eval_op_add_float(env, e->cdr, (float)partial + e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: addint: attempt to add a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_add(env_t *env, const expr_t *e)
{
  return eval_op_add_int(env, e, 0);
}

static expr_t *eval_op_sub_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: subfloat: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_float(env, e->cdr, partial - (float)e1->intval); break;
  case FLOAT_T: r = eval_op_sub_float(env, e->cdr, partial - e1->floatval); break;
  default: { 
    r = _err_expr(0, "eval: subfloat: attempt to substract a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_sub_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    return _err_expr(0, "eval: subint: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_int(env, e->cdr, partial - e1->intval); break;
  case FLOAT_T: r = eval_op_sub_float(env, e->cdr, (float)partial - e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: subint: attempt to subtract a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_sub(env_t *env, const expr_t *e)
{
  if(NO_ARGS(e)) return _int_expr(0);
  if(!(IS_LIST(e))) {
    return _err_expr(0, "eval: sub: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: r = eval_op_sub_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: sub: attempt to subtract a non-numeric value", 0);
  }
  }
  
  // if we only had one operand, we want to return the negative of it
  if(e->cdr == &NIL) {
    switch(e1->type) {
    case INT_T: r->intval = -r->intval; break;
    case FLOAT_T: r->floatval = -r->intval; break;
    }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_mul_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: mulfloat: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_mul_float(env, e->cdr, partial * (float)e1->intval); break;
  case FLOAT_T: r = eval_op_mul_float(env, e->cdr, partial * e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: mulfloat: attempt to multiply a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_mul_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: mulint: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_mul_int(env, e->cdr, partial * e1->intval); break;
  case FLOAT_T: r = eval_op_mul_float(env, e->cdr, (float)partial * e1->floatval); break;
  default: { 
    r = _err_expr(0, "eval: mulint: attempt to multiply a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_mul(env_t *env, const expr_t *e)
{
  return eval_op_mul_int(env, e, 1);
}

static expr_t *eval_op_div_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: divfloat: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_float(env, e->cdr, partial / (float)e1->intval); break;
  case FLOAT_T: r = eval_op_div_float(env, e->cdr, partial / e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: divfloat: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_div_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    return _err_expr(0, "eval: divint: unexpected argument type", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_int(env, e->cdr, partial / e1->intval); break;
  case FLOAT_T: r = eval_op_div_float(env, e->cdr, (float)partial / e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: divint: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_div(env_t *env, const expr_t *e)
{
  // this is a bit inefficient, but makes things easy to read
  if(NO_ARGS(e)) { 
    return _err_expr(0, "eval: div: need at least two arguments for division", 0); 
  }
  if(ONE_ARGS(e)) { 
    return _err_expr(0, "eval: div: need at least two arguments for division", 0); 
  }

  expr_t *e1 = e->car->eval(env, e->car), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: r = eval_op_div_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    r = _err_expr(0, "eval: div: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_car(env_t *env, const expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *r;

  if(!IS_LIST(e1)) {
    r = _err_expr(0, "eval: car: requires list argument", 0);
  } else {
    r = _clone_expr(e1->car);
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_cdr(env_t *env, const expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *r;

  if(!IS_LIST(e1)) {
    r = _err_expr(0, "eval: cdr: requires list argument", 0);
  } else {
    r = _clone_expr(e1->cdr);
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_cons(env_t *env, const expr_t *e)
{
  if(!TWO_ARGS(e)) {
    return _err_expr(0, "eval: error: cons: requires exactly 2 arguments", 0);
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(IS_ERR(e1)) { return e1; }
  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(IS_ERR(e2)) { _free_expr(e1); return e2; }

  return _list_expr(e1, e2);
}

expr_t *eval_op_list(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &NIL;

  expr_t *e1 = e->car->eval(env, e->car);
  if(IS_ERR(e1)) { return e1; }
  expr_t *e2 = eval_op_list(env, e->cdr);
  if(IS_ERR(e2)) { _free_expr(e1); return e2; }
  
  return _list_expr(e1, e2);
}

expr_t *eval_op_append(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &NIL;

  expr_t *r = e->car->eval(env, e->car);

  if(r == &NIL) return eval_op_append(env, e->cdr);
  if(e->cdr == &NIL) return r;

  if(!IS_LIST(r)) {
    _free_expr(r);
    return _err_expr(0, "eval: append: first argument of append must be a list", 0);
  }

  expr_t *e2 = eval_op_append(env, e->cdr), *ptr;

  // find the tail and append
  for(ptr = r; ptr->cdr != &NIL; ptr = ptr->cdr);
  ptr->cdr = e2;

  return r;
}

expr_t *eval_op_quote(env_t *env, const expr_t *e)
{
  return _clone_expr(e->car);
}

expr_t *eval_op_let(env_t *env, const expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    return _err_expr(0, "eval: let: incorrect number of arguments", 0);
  }

  expr_t *defs = e->car, *body = e->cdr->car, *d_ptr = defs, *tmp, *r;

  env_t letenv;
  init_env(&letenv, env);

  // for(d_ptr = defs; d_ptr != &NIL; d_ptr = d_ptr->cdr) {
  while(d_ptr != &NIL) {
    expr_t *def = d_ptr->car;
    expr_t *id = def->car;
    expr_t *value = def->cdr->car->eval(env, def->cdr->car);

    if(IS_ERR(value)) {
      free_env(&letenv);
      return _err_expr(value, "eval: let: malformed let expression", 0);
    }

    put(&letenv, def->car, value);
    _free_expr(value);

    tmp = d_ptr->cdr;
    d_ptr = tmp;
  }

  r = body->eval(&letenv, body);

  free_env(&letenv);
  return r;
}

expr_t *eval_op_if(env_t *env, const expr_t *e)
{
  // TODO test for either two or three args
  expr_t *cond = e->car->eval(env, e->car);
  expr_t *r;

  if(cond) {
    if(!IS_BOOL(cond)) {
      r = _err_expr(0, "eval: if: boolean value expected", 0);
    } else if(cond == &T) {
      r = e->cdr->car->eval(env, e->cdr->car);
    } else if(e->cdr->cdr != &NIL) { // cond == &F, else
      r = e->cdr->cdr->car->eval(env, e->cdr->cdr->car);
    } else { // cond == &F, no "else"
      r = &NIL;
    }
    _free_expr(cond);
  }

  return r;
}

expr_t *eval_op_not(env_t *env, const expr_t *e)
{
  if(!(ONE_ARGS(e))) {
    return _err_expr(0, "eval: not: requires exactly 1 argument", 0);
  }
  
  expr_t *b = e->car->eval(env, e->car), *r;
  if(!IS_BOOL(b)) {
    r = _err_expr(0, "eval: not: boolean value expected", 0);
    _free_expr(b);
  } else {
    r = (b == &T) ? &F : &T;
  }
  
  return r;
}

expr_t *eval_op_and(env_t *env, const expr_t *e)
{
  for(const expr_t *ptr = e; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car);
    if(!IS_BOOL(b)) {
      _free_expr(b);
      return _err_expr(0, "eval: and: boolean value expected", 0);
    } else if(b != &T) {
      return &F;
    } 
  }
  return &T;
}

expr_t *eval_op_or(env_t *env, const expr_t *e)
{
  for(const expr_t *ptr = e; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car);
    if(!IS_BOOL(b)) {
      _free_expr(b);
      return _err_expr(0, "eval: or: boolean value expected", 0);
    } else if(b == &T) {
      return &T;
    } 
  }
  return &F;
}

expr_t *eval_op_equal(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != 0) return &F;
  }
  return &T;
}

expr_t *eval_op_lt(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != -1) return &F;
  }
  return &T;
}

expr_t *eval_op_le(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r > 0) return &F;
  }
  return &T;
}

expr_t *eval_op_gt(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != 1) return &F;
  }
  return &T;
}

expr_t *eval_op_ge(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r < 0) return &F;
  }
  return &T;
}

expr_t *eval_op_substr(env_t *env, const expr_t *e)
{
  expr_t *strexpr, *posexpr, *lenexpr;
  int pos, len;

  if(e == &NIL) {
    fprintf(stderr, "eval: error: substr: no string provided\n");
    return 0;
  }

  strexpr = e->car->eval(env, e->car);
  if(!IS_STRING(strexpr)) {
    fprintf(stderr, "eval: error: substr: first argument must be a string\n");
    _free_expr(strexpr);
    return 0;
  }

  if(e->cdr == &NIL) return strexpr;

  posexpr = e->cdr->car->eval(env, e->cdr->car);
  if(!IS_INT(posexpr)) {
    fprintf(stderr, "eval: error: substr: position argument must be an integer\n");
    _free_expr(strexpr);
    _free_expr(posexpr);
    return 0;
  }
  pos = posexpr->intval;
  _free_expr(posexpr);

  len = strlen(strexpr->strval);
  if(pos > len) {
    fprintf(stderr, "eval: error: substr: position argument larger than string length\n");
    _free_expr(strexpr);
    return 0;
  }

  if(e->cdr->cdr == &NIL) {
    len = len - pos;
  } else {
    lenexpr = e->cdr->cdr->car->eval(env, e->cdr->cdr->car);
    if(!IS_INT(lenexpr)) {
      fprintf(stderr, "eval: error: substr: length argument must be an integer\n");
      _free_expr(strexpr);
      _free_expr(lenexpr);
      return 0;
    }
    len = lenexpr->intval;
    _free_expr(lenexpr);

    if((pos+len) > strlen(strexpr->strval)) {
      fprintf(stderr, "eval: error: substr: position+length arguments greater than string length\n");
      _free_expr(strexpr);
      return 0;
    }
  }

  char *str = calloc(len+1, sizeof(char));
  strncpy(str, strexpr->strval+pos, len);

  _free_expr(strexpr);
  return _str_expr(str);
}

expr_t *eval_op_strlen(env_t *env, const expr_t *e)
{
  if(!ONE_ARGS(e)) {
    fprintf(stderr, "eval: error: strlen: takes exactly one argument\n");
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  
  if(!IS_STRING(e1)) {
    fprintf(stderr, "eval: error: strlen: argument is not a string\n");
    return 0;
  }

  expr_t *r = _int_expr(strlen(e1->strval));
  _free_expr(e1);
  return r;
}

expr_t *eval_op_split(env_t *env, const expr_t *e)
{
  if(!TWO_ARGS(e)) {
    fprintf(stderr, "eval: error: split: takes exactly two argument\n");
    return 0;
  }

  expr_t *strexpr = e->car->eval(env, e->car);
  if(!IS_STRING(strexpr)) {
    fprintf(stderr, "eval: error: split: first argument must be a string\n");
    _free_expr(strexpr);
    return 0;
  }

  expr_t *splitexpr = e->cdr->car->eval(env, e->cdr->car);
  if(!IS_STRING(splitexpr)) {
    fprintf(stderr, "eval: error: split: second argument must be a string\n");
    _free_expr(strexpr);
    _free_expr(splitexpr);
    return 0;
  }
  
  int len = strlen(strexpr->strval);
  char *str = strexpr->strval, *split = splitexpr->strval;

  expr_t *r = &NIL, *tail;
  char *p = strtok(str, split);
  if(p) tail = (r = _list_expr(_str_expr(strdup(p)), &NIL));
  while(p = strtok(0, split)) {
    tail->cdr = _list_expr(_str_expr(strdup(p)), &NIL);
    tail = tail->cdr;
  }

  _free_expr(strexpr);
  _free_expr(splitexpr);
  return r;
}

expr_t *eval_op_openif(env_t *env, const expr_t *e)
{
  if(!ONE_ARGS(e)) {
    fprintf(stderr, "eval: error: openif: requires exactly 1 string argument\n");
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!IS_STRING(e1)) {
    fprintf(stderr, "eval: error: openif: requires exactly 1 string argument\n");
    _free_expr(e1);
    return 0;
  }

  FILE *fp = fopen(e1->strval, "r");
  if(!fp) {
    fprintf(stderr, "eval: error: openif: error while opening file: %s\n", strerror(errno));
    _free_expr(e1);
    return 0;
  }

  _free_expr(e1);
  return _port_expr(fp);
}

expr_t *eval_op_closeif(env_t *env, const expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *r;
  if(!IS_PORT(e1) || e->cdr != &NIL) {
    r = _err_expr(e1, "eval: closeif: requires exactly 1 port argument", 0);
  } else {
    fclose(e1->fp);
    r = &NIL;
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_readline(env_t *env, const expr_t *e)
{
  if(!ONE_ARGS(e)) {
    fprintf(stderr, "eval: error: readline: requires exactly 1 port argument\n");
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!IS_PORT(e1)) {
    fprintf(stderr, "eval: error: readline: requires exactly 1 port argument\n");
    _free_expr(e1);
    return 0;
  }

  char *str = calloc(_PORT_IO_BUF_SIZE, sizeof(char));
  expr_t *r;
  // TODO grow the buffer and continue to read until a newline or EOF is reached
  if(fgets(str, _PORT_IO_BUF_SIZE, e1->fp)) {
    // trim the newline
    if(str[strlen(str)-1] == '\n') str[strlen(str)-1] = 0;
    // TODO trim the buffer if the size is > strlen
    r = _str_expr(str);
  } else {
    r = &NIL;
  }

  _free_expr(e1);
  return r;
}
