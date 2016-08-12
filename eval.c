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

expr_t *eval_idem(env_t *env, const expr_t *e)
{
  return _clone_expr(e);
}

expr_t *eval_op_define(env_t *env, const expr_t *e)
{
  // we are expecting two arguments
  if(!(TWO_ARGS(e))) { 
    fprintf(stderr, "eval: error: define: expects exactly 2 arguments\n");
    return 0; 
  }

  expr_t *id = e->car;
  if(!IS_IDENT(id)) {
    fprintf(stderr, "eval: error: define: first argument must be an identifier\n");
  }
  
  expr_t *value = e->cdr->car->eval(env, e->cdr->car);
  if(value) {
    put(env, id, value);
  } else {
    id = 0;
  }

  _free_expr(value);
  return _clone_expr(id);
}

expr_t *eval_op_lambda(env_t *env, const expr_t *e)
{
  return _fun_expr(_clone_expr(e->car), _clone_expr(e->cdr));
}

expr_t *eval_fun(env_t *env, const expr_t *e)
{
  fprintf(stderr, "eval_fun\n");
  return 0;
}

static expr_t *eval_function_call(env_t *env, expr_t *formals, expr_t *body, expr_t *args)
{
  expr_t *f_ptr = formals, *a_ptr = args, *tmp, *result;

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
    fprintf(stderr, "eval: error: incorrect number of arguments to function\n");
    // TODO cleanup?
    return 0;
  } else {
    result = body->eval(&funenv, body);
  }

  // clean up and return
  free_env(&funenv);

  return result;
}

expr_t *eval_list(env_t *env, const expr_t *e)
{
  expr_t *result;
  if(IS_OP(e->car)) {
    // we already know how to execute ops
    result = e->car->eval(env, e->cdr);
  } else {
    expr_t *fun = e->car->eval(env, e->car);
    if(!IS_FUN(fun)) {
      fprintf(stderr, "eval: error: list: neither an operation nor a function\n");
      result = 0;
    } else {
      result = eval_function_call(env, fun->car, fun->cdr->car, e->cdr);
    }
    _free_expr(fun);
  }

  return result;
}

expr_t *eval_ident(env_t *env, const expr_t *e)
{
  expr_t *result = get(env, e);
  if(!result) fprintf(stderr, "eval: error: unbound variable\n");
  return result;
}

static expr_t *eval_op_add_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "eval: add: unexpected argument type\n");
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_add_float(env, e->cdr, partial + (float)e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, partial + e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to add to a non-numeric value");
    result = 0;
  }
  }

  _free_expr(e1);
  return result;
}

static expr_t *eval_op_add_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    fprintf(stderr, "eval: add: unexpected argument type\n");
    return 0; 
  }

  // we'll use this to hold the result
  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_add_int(env, e->cdr, partial + e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, (float)partial + e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to add a non-numeric value");
    result = 0;
  }
  }

  _free_expr(e1);
  return result;
}

expr_t *eval_op_add(env_t *env, const expr_t *e)
{
  return eval_op_add_int(env, e, 0);
}

static expr_t *eval_op_sub_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to subtract a non-numeric value");
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_sub_float(env, e->cdr, partial - (float)e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, partial - e1->floatval); break;
  default: { 
    fprintf(stderr, "attempt to subtract a non-numeric value");
    result = 0;
  }
  }

  _free_expr(e1);
  return result;
}

static expr_t *eval_op_sub_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    fprintf(stderr, "attempt to subtract a non-numeric value"); 
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_sub_int(env, e->cdr, partial - e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)partial - e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to subtract a non-numeric value");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

expr_t *eval_op_sub(env_t *env, const expr_t *e)
{
  if(NO_ARGS(e)) return _int_expr(0);
  if(!(IS_LIST(e))) {
    fprintf(stderr, "eval: error: attempt to subtract a non-numeric value"); 
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_sub_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    fprintf(stderr, "eval: error: attempt to subtract a non-numeric value");
    return 0;
  }
  }
  
  // if we only had one operand, we want to return the negative of it
  if(e->cdr == &NIL) {
    switch(e1->type) {
    case INT_T: result->intval = -result->intval; break;
    case FLOAT_T: result->floatval = -result->intval; break;
    }
  }

  _free_expr(e1);
  return result;
}

static expr_t *eval_op_mul_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to multiply a non-numeric value"); 
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_mul_float(env, e->cdr, partial * (float)e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, partial * e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to multiply a non-numeric value");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

static expr_t *eval_op_mul_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to multiply a non-numeric value"); 
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_mul_int(env, e->cdr, partial * e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, (float)partial * e1->floatval); break;
  default: { 
    fprintf(stderr, "attempt to multiply a non-numeric value");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

expr_t *eval_op_mul(env_t *env, const expr_t *e)
{
  return eval_op_mul_int(env, e, 1);
}

static expr_t *eval_op_div_float(env_t *env, const expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to divide a non-numeric value"); 
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_div_float(env, e->cdr, partial / (float)e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, partial / e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to divide a non-numeric value");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

static expr_t *eval_op_div_int(env_t *env, const expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to divide a non-numeric value"); 
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_div_int(env, e->cdr, partial / e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)partial / e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to divide a non-numeric value");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

expr_t *eval_op_div(env_t *env, const expr_t *e)
{
  // this is a bit inefficient, but makes things easy to read
  if(NO_ARGS(e)) { 
    fprintf(stderr, "eval: error: div: need at least two arguments for division\n"); 
    return 0; 
  }
  if(ONE_ARGS(e)) { 
    fprintf(stderr, "eval: error: div: need at least two arguments for division\n");
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INT_T: result = eval_op_div_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    fprintf(stderr, "eval: error: div: attempt to divide a non-numeric value\n");
    return 0;
  }
  }

  _free_expr(e1);
  return result;
}

expr_t *eval_op_car(env_t *env, const expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *result;

  if(!IS_LIST(e1)) {
    fprintf(stderr, "eval: error: car: requires list argument\n");
    return 0;
  }

  result = _clone_expr(e1->car);

  _free_expr(e1);
  return result;
}

expr_t *eval_op_cdr(env_t *env, const expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *result;

  if(!IS_LIST(e1)) {
    fprintf(stderr, "eval: error: car: requires list argument\n");
    return 0;
  }

  result = _clone_expr(e1->cdr);

  _free_expr(e1);
  return result;
}

expr_t *eval_op_cons(env_t *env, const expr_t *e)
{
  if(!TWO_ARGS(e)) {
    fprintf(stderr, "eval: error: cons: requires exactly 2 arguments\n");
    return 0;
  }
  expr_t *e1 = e->car->eval(env, e->car);
  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);

  expr_t *result = _list_expr(e1, e2);

  return result;
}

expr_t *eval_op_list(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &NIL;

  expr_t *car = e->car->eval(env, e->car);
  if(!car) {
    return 0;
  }

  expr_t *cdr = eval_op_list(env, e->cdr);
  if(!cdr) {
    return 0;
  }
  
  return _list_expr(car, cdr);
}

expr_t *eval_op_append(env_t *env, const expr_t *e)
{
  // TODO make append accept any number of args
  if(!(TWO_ARGS(e))) { 
    fprintf(stderr, "eval: error: append: incorrect number of arguments"); 
    return 0; 
  }

  // TODO null check!
  expr_t *l1 = e->car->eval(env, e->car);
  expr_t *l2 = e->cdr->car->eval(env, e->cdr->car);

  if(l1 == &NIL) return l2;

  expr_t *ptr;
  for(ptr = l1; ptr->cdr != &NIL; ptr = ptr->cdr){}

  ptr->cdr = l2;
  
  return l1;
}

expr_t *eval_op_quote(env_t *env, const expr_t *e)
{
  if(!(ONE_ARGS(e))) { 
    fprintf(stderr, "eval: error: quote: incorrect number of arguments"); 
    return 0; 
  }
  
  expr_t *result = _clone_expr(e->car);

  return result;
}

expr_t *eval_op_let(env_t *env, const expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: let: incorrect number of arguments"); 
    return 0; 
  }

  expr_t *defs = e->car, *body = e->cdr->car, *d_ptr = defs, *tmp, *result;

  env_t letenv;
  init_env(&letenv, env);

  // for(d_ptr = defs; d_ptr != &NIL; d_ptr = d_ptr->cdr) {
  while(d_ptr != &NIL) {
    expr_t *def = d_ptr->car;
    expr_t *id = def->car;
    expr_t *value = def->cdr->car->eval(env, def->cdr->car);

    if(!value) {
      return 0;
    }

    put(&letenv, def->car, value);
    _free_expr(value);

    tmp = d_ptr->cdr;
    d_ptr = tmp;
  }

  result = body->eval(&letenv, body);

  free_env(&letenv);
  return result;
}

expr_t *eval_op_if(env_t *env, const expr_t *e)
{
  // TODO test for either two or three args
  expr_t *cond = e->car->eval(env, e->car);
  expr_t *result;

  if(cond) {
    if(!IS_BOOL(cond)) {
      fprintf(stderr, "eval: error: if: boolean value expected\n");
      result = 0;
    } else if(cond == &T) {
      result = e->cdr->car->eval(env, e->cdr->car);
    } else if(e->cdr->cdr != &NIL) { // cond == &F, else
      result = e->cdr->cdr->car->eval(env, e->cdr->cdr->car);
    } else { // cond == &F, no "else"
      result = &NIL;
    }
    _free_expr(cond);
  }

  return result;
}

expr_t *eval_op_not(env_t *env, const expr_t *e)
{
  if(!(ONE_ARGS(e))) {
    fprintf(stderr, "eval: error: not: incorrect number of arguments");
    return 0;
  }
  
  expr_t *b = e->car->eval(env, e->car), *result;
  if(!IS_BOOL(b)) {
    fprintf(stderr, "not: boolean value expected");
    _free_expr(b);
    result = 0;
  } else {
    result = (b == &T) ? &F : &T;
  }
  
  return result;
}

expr_t *eval_op_and(env_t *env, const expr_t *e)
{
  for(const expr_t *ptr = e; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car);
    if(!IS_BOOL(b)) {
      fprintf(stderr, "eval:error: and: expected boolean value\n");
      _free_expr(b);
      return 0;
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
      fprintf(stderr, "eval:error: and: expected boolean value\n");
      _free_expr(b);
      return 0;
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
    int result = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(result != 0) return &F;
  }
  return &T;
}

expr_t *eval_op_lt(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int result = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(result != -1) return &F;
  }
  return &T;
}

expr_t *eval_op_le(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int result = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(result > 0) return &F;
  }
  return &T;
}

expr_t *eval_op_gt(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int result = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(result != 1) return &F;
  }
  return &T;
}

expr_t *eval_op_ge(env_t *env, const expr_t *e)
{
  if(e == &NIL) return &T;
  for(const expr_t *ptr = e; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car);
    int result = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(result < 0) return &F;
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

  expr_t *result = _int_expr(strlen(e1->strval));
  _free_expr(e1);
  return result;
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
  return &T;
}
