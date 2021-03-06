#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include "punky.h"
#include "types.h"
#include "env.h"
#include "eval.h"

/* static convenience functions */
static expr_t *eval_op_add_float(expr_t *env, float partial, const expr_t *args);
static expr_t *eval_op_add_int(expr_t *env, int partial, const expr_t *args);
static expr_t *eval_op_sub_float(expr_t *env, float partial, const expr_t *args);
static expr_t *eval_op_sub_int(expr_t *env, int partial, const expr_t *args);
static expr_t *eval_op_mul_float(expr_t *env, float partial, const expr_t *args);
static expr_t *eval_op_mul_int(expr_t *env, int partial, const expr_t *args);
static expr_t *eval_op_div_float(expr_t *env, float partial, const expr_t *args);
static expr_t *eval_op_div_int(expr_t *env, int partial, const expr_t *args);
static expr_t *eval_function_call(expr_t *env, const expr_t *formals, const expr_t *body, const expr_t *args);

expr_t *eval_clone(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args) {
    // TODO this error message sucks
    return _err_expr(0, "eval: clone: attempt to provide args to a clonable", 0);
  }
  return _clone_expr(self);
}

expr_t *eval_op_define(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *id = args->car;
  if(!IS_IDENT(id)) {
    return _err_expr(id, "eval: define: first argument must be an identifier", 0);
  }
  
  expr_t *value = args->cdr->car->eval(env, args->cdr->car, 0), *r;
  if(!IS_ERR(value)) {
    put(env, id->strval, value);
    _free_expr(value);
    r = _id_expr(strdup(id->strval));
  } else {
    r = _err_expr(value, "eval: define: second argument evaluated to an error", 0);
  }

  return r;
}

expr_t *eval_op_defun(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *id = args->car;
  if(!IS_IDENT(id)) {
    return _err_expr(id, "eval: defun: first argument must be an identifier", 0);
  }

  expr_t *fundef = _fun_expr(_clone_expr(args->cdr->car), _clone_expr(args->cdr->cdr->car));
  put(env, id->strval, fundef);
  _free_expr(fundef);

  return _id_expr(strdup(id->strval));
}

expr_t *eval_op_lambda(expr_t *env, const expr_t *self, const expr_t *args)
{
  return _fun_expr(_clone_expr(args->car), _clone_expr(args->cdr->car));
}

expr_t *eval_fun(expr_t *env, const expr_t *self, const expr_t *args)
{
  return eval_function_call(env, self->car, self->cdr, args);
}

static expr_t *eval_function_call(expr_t *env, const expr_t *formals, const expr_t *body, const expr_t *args)
{
  const expr_t *f_ptr = formals, *a_ptr = args, *tmp;
  expr_t *r;

  expr_t *funenv = new_env(env);
  
  // evaluate each arg and bind it to its formal parameter in the new env we've created
  while((f_ptr != &NIL) && (a_ptr != &NIL)) {
   expr_t *val = a_ptr->car->eval(env, a_ptr->car, 0) ;
    // TODO validate IS_IDENT
    put(funenv, f_ptr->car->strval, val);
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
   r = body->eval(funenv, body, 0);
  }

  // clean up and return
  free_env(funenv);
  return r;
}

expr_t *eval_list(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *fun = self->car->eval(env, self->car, self->cdr), *r;
  if(!IS_ERR(fun) && args) {
    r = fun->eval(env, fun, args);
    _free_expr(fun);
  } else {
    r = fun;
  }
  return r;
}

expr_t *eval_ident(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *fun = get(env, self->strval), *r;
  if(!IS_ERR(fun) && args) {
    r = fun->eval(env, fun, args);
    _free_expr(fun);
  } else {
    r = fun;
  }
  return r;
}

static expr_t *eval_op_add_float(expr_t *env, float partial, const expr_t *args)
{
  if(args == &NIL) return _float_expr(partial);
  if(args->type != LIST_T) { 
    return _err_expr(0, "eval: addfloat: unexpected argument type", 0);
  }

 expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_add_float(env, partial + (float)e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_add_float(env, partial + e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: addfloat: attempt to add a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_add_int(expr_t *env, int partial, const expr_t *args)
{
  if(args == &NIL) return _int_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: addint: unexpected argument type", 0);
  }

  // we'll use this to hold the result
  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_add_int(env, partial + e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_add_float(env, (float)partial + e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: addint: attempt to add a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_add(expr_t *env, const expr_t *self, const expr_t *args)
{
  return eval_op_add_int(env, 0, args);
}

static expr_t *eval_op_sub_float(expr_t *env, float partial, const expr_t *args)
{
  if(args == &NIL) return _float_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: subfloat: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_float(env, partial - (float)e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_sub_float(env, partial - e1->floatval, args->cdr); break;
  default: { 
    r = _err_expr(0, "eval: subfloat: attempt to subtract a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_sub_int(expr_t *env, int partial, const expr_t *args)
{
  if(args == &NIL) return _int_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: subint: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_int(env, partial - e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_sub_float(env, (float)partial - e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: subint: attempt to subtract a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_sub(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(NO_ARGS(args)) return _int_expr(0);
  if(!(IS_LIST(args))) {
    return _err_expr(0, "eval: sub: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_sub_int(env, e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_sub_float(env, (float)e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: sub: attempt to subtract a non-numeric value", 0);
  }
  }
  
  // if we only had one operand, we want to return the negative of it
  if(args->cdr == &NIL) {
    switch(e1->type) {
    case INT_T: r->intval = -r->intval; break;
    case FLOAT_T: r->floatval = -r->intval; break;
    }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_mul_float(expr_t *env, float partial, const expr_t *args)
{
  if(args == &NIL) return _float_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: mulfloat: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_mul_float(env, partial * (float)e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_mul_float(env, partial * e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: mulfloat: attempt to multiply a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_mul_int(expr_t *env, int partial, const expr_t *args)
{
  if(args == &NIL) return _int_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: mulint: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_mul_int(env, partial * e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_mul_float(env, (float)partial * e1->floatval, args->cdr); break;
  default: { 
    r = _err_expr(0, "eval: mulint: attempt to multiply a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_mul(expr_t *env, const expr_t *self, const expr_t *args)
{
  return eval_op_mul_int(env, 1, args);
}

static expr_t *eval_op_div_float(expr_t *env, float partial, const expr_t *args)
{
  if(args == &NIL) return _float_expr(partial);
  if(args->type != LIST_T) { 
    return _err_expr(0, "eval: divfloat: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_float(env, partial / (float)e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_div_float(env, partial / e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: divfloat: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

static expr_t *eval_op_div_int(expr_t *env, int partial, const expr_t *args)
{
  if(args == &NIL) return _int_expr(partial);
  if(args->type != LIST_T) {
    return _err_expr(0, "eval: divint: unexpected argument type", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_int(env, partial / e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_div_float(env, (float)partial / e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: divint: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_div(expr_t *env, const expr_t *self, const expr_t *args)
{
  // this is a bit inefficient, but makes things easy to read
  if(NO_ARGS(args)) {
    return _err_expr(0, "eval: div: need at least two arguments for division", 0); 
  }
  if(ONE_ARGS(args)) {
    return _err_expr(0, "eval: div: need at least two arguments for division", 0); 
  }

  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  switch(e1->type) {
  case INT_T: r = eval_op_div_int(env, e1->intval, args->cdr); break;
  case FLOAT_T: r = eval_op_div_float(env, (float)e1->floatval, args->cdr); break;
  default: {
    r = _err_expr(0, "eval: div: attempt to divide a non-numeric value", 0);
  }
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_mod(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!TWO_ARGS(args)) {
    return _err_expr(0, "eval: mod: requires exactly two arguments", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0);
  int v1;
  switch(e1->type) {
  case INT_T: v1 = e1->intval; break;
  case FLOAT_T: v1 = (int)e1->floatval; break;
  default: {
    _free_expr(e1);
    return _err_expr(0, "eval: mod: attempt to mod a non-numeric value (numerator)", 0);
  }
  }

 expr_t *e2 = args->cdr->car->eval(env, args->cdr->car, 0);
  int v2;
  switch(e2->type) {
  case INT_T: v2 = e2->intval; break;
  case FLOAT_T: v2 = (int)e2->floatval; break;
  default: {
    _free_expr(e1);
    _free_expr(e2);
    return _err_expr(0, "eval: mod: attempt to mod a non-numeric value (denominator)", 0);
  }
  }

  expr_t *r = _int_expr(v1 % v2);
  _free_expr(e1);
  _free_expr(e2);

  return r;
}

expr_t *eval_op_pow(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!TWO_ARGS(args)) {
    return _err_expr(0, "eval: pow: requires exactly two arguments", 0);
  }

  int doubles = 0;
  
  expr_t *e1 = args->car->eval(env, args->car, 0);
  double v1;
  switch(e1->type) {
  case INT_T: v1 = (double)e1->intval; break;
  case FLOAT_T: v1 = e1->floatval; doubles = 1; break;
  default: {
    _free_expr(e1);
    return _err_expr(0, "eval: pow: attempt to raise a non-numeric value", 0);
  }
  }

  expr_t *e2 = args->cdr->car->eval(env, args->cdr->car, 0);
  double v2;
  switch(e2->type) {
  case INT_T: v2 = (double)e2->intval; break;
  case FLOAT_T: v2 = e2->floatval; doubles = 1; break;
  default: {
    _free_expr(e1);
    _free_expr(e2);
    return _err_expr(0, "eval: pow: attempt to raise a value to a non-numeric power", 0);
  }
  }

  double result = pow(v1, v2);
  _free_expr(e1);
  _free_expr(e2);

  return doubles ? _float_expr(result) : _int_expr((int)result);
}

expr_t *eval_op_car(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *e1 = args->car->eval(env, args->car, 0), *r;

  if(!IS_LIST(e1)) {
    r = _err_expr(0, "eval: car: requires list argument", 0);
  } else {
    r = _clone_expr(e1->car);
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_cdr(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *e1 = args->car->eval(env, args->car, 0), *r;

  if(!IS_LIST(e1)) {
    r = _err_expr(0, "eval: cdr: requires list argument", 0);
  } else {
    r = _clone_expr(e1->cdr);
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_cons(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!TWO_ARGS(args)) {
    return _err_expr(0, "eval: cons: requires exactly 2 arguments", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0);
  if(IS_ERR(e1)) { return e1; }
  expr_t *e2 = args->cdr->car->eval(env, args->cdr->car, 0);
  if(IS_ERR(e2)) { _free_expr(e1); return e2; }

  return _list_expr(e1, e2);
}

expr_t *eval_op_list(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &NIL;

  expr_t *e1 = args->car->eval(env, args->car, 0);
  if(IS_ERR(e1)) { return e1; }
  expr_t *e2 = eval_op_list(env, 0, args->cdr);
  if(IS_ERR(e2)) { _free_expr(e1); return e2; }
  
  return _list_expr(e1, e2);
}

expr_t *eval_op_append(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &NIL;

  expr_t *r = args->car->eval(env, args->car, 0);

  if(r == &NIL) return eval_op_append(env, 0, args->cdr);
  if(args->cdr == &NIL) return r;

  if(!IS_LIST(r)) {
    _free_expr(r);
    return _err_expr(0, "eval: append: first argument of append must be a list", 0);
  }

  expr_t *e2 = eval_op_append(env, 0, args->cdr), *ptr;

  // find the tail and append
  for(ptr = r; ptr->cdr != &NIL; ptr = ptr->cdr);
  ptr->cdr = e2;

  return r;
}

expr_t *eval_op_quote(expr_t *env, const expr_t *self, const expr_t *args)
{
  return _clone_expr(args->car);
}

expr_t *eval_op_let(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!(TWO_ARGS(args))) {
    return _err_expr(0, "eval: let: incorrect number of arguments", 0);
  }

  expr_t *defs = args->car, *body = args->cdr->car, *d_ptr = defs, *tmp, *r;

  expr_t *letenv = new_env(env);

  // for(d_ptr = defs; d_ptr != &NIL; d_ptr = d_ptr->cdr) {
  while(d_ptr != &NIL) {
    expr_t *def = d_ptr->car;
    expr_t *value = def->cdr->car->eval(env, def->cdr->car, 0);

    if(IS_ERR(value)) {
      free_env(letenv);
      return _err_expr(value, "eval: let: malformed let expression", 0);
    }

    // TODO validate IS_IDENT
    put(letenv, def->car->strval, value);
    _free_expr(value);

    tmp = d_ptr->cdr;
    d_ptr = tmp;
  }

  r = body->eval(letenv, body, 0);

  free_env(letenv);
  return r;
}

expr_t *eval_op_if(expr_t *env, const expr_t *self, const expr_t *args)
{
  // TODO test for either two or three args
  expr_t *cond = args->car->eval(env, args->car, 0);
  expr_t *r;

  if(cond) {
    if(!IS_BOOL(cond)) {
      r = _err_expr(0, "eval: if: boolean value expected", 0);
    } else if(cond == &T) {
     r = args->cdr->car->eval(env, args->cdr->car, 0);
    } else if(args->cdr->cdr != &NIL) { // cond == &F, else
     r = args->cdr->cdr->car->eval(env, args->cdr->cdr->car, 0);
    } else { // cond == &F, no "else"
      r = &NIL;
    }
    _free_expr(cond);
  }

  return r;
}

expr_t *eval_op_not(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!(ONE_ARGS(args))) {
    return _err_expr(0, "eval: not: requires exactly 1 argument", 0);
  }
  
  expr_t *b = args->car->eval(env, args->car, 0), *r;
  if(!IS_BOOL(b)) {
    r = _err_expr(0, "eval: not: boolean value expected", 0);
    _free_expr(b);
  } else {
    r = (b == &T) ? &F : &T;
  }
  
  return r;
}

expr_t *eval_op_and(expr_t *env, const expr_t *self, const expr_t *args)
{
  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car, 0);
    if(!IS_BOOL(b)) {
      _free_expr(b);
      return _err_expr(0, "eval: and: boolean value expected", 0);
    } else if(b != &T) {
      return &F;
    } 
  }
  return &T;
}

expr_t *eval_op_or(expr_t *env, const expr_t *self, const expr_t *args)
{
  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car, 0);
    if(!IS_BOOL(b)) {
      _free_expr(b);
      return _err_expr(0, "eval: or: boolean value expected", 0);
    } else if(b == &T) {
      return &T;
    } 
  }
  return &F;
}

expr_t *eval_op_xor(expr_t *env, const expr_t *self, const expr_t *args)
{
  int v = 0;
  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *b = ptr->car->eval(env, ptr->car, 0);
    if(!IS_BOOL(b)) {
      _free_expr(b);
      return _err_expr(0, "eval: xor: boolean value expected", 0);
    } else {
      v = v ^ (b == &T ? 1 : 0);
    }
  }

  return v ? &T : &F;
}

expr_t *eval_op_equal(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &T;
  for(const expr_t *ptr = args; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car, 0),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car, 0);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != 0) return &F;
  }
  return &T;
}

expr_t *eval_op_lt(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &T;
  for(const expr_t *ptr = args; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car, 0),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car, 0);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != -1) return &F;
  }
  return &T;
}

expr_t *eval_op_le(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &T;
  for(const expr_t *ptr = args; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car, 0),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car, 0);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r > 0) return &F;
  }
  return &T;
}

expr_t *eval_op_gt(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &T;
  for(const expr_t *ptr = args; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car, 0),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car, 0);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r != 1) return &F;
  }
  return &T;
}

expr_t *eval_op_ge(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return &T;
  for(const expr_t *ptr = args; ptr->cdr != &NIL; ptr = ptr->cdr) {
    expr_t *e1 = ptr->car->eval(env, ptr->car, 0),
      *e2 = ptr->cdr->car->eval(env, ptr->cdr->car, 0);
    int r = compare(e1, e2);
    _free_expr(e1); _free_expr(e2);
    if(r < 0) return &F;
  }
  return &T;
}

expr_t *eval_op_substr(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *strexpr, *posexpr, *lenexpr;
  int pos, len;

  if(args == &NIL) {
    return _err_expr(0, "eval: substr: no string provided", 0);
  }

 strexpr = args->car->eval(env, args->car, 0);
  if(!IS_STRING(strexpr)) {
    _free_expr(strexpr);
    return _err_expr(0, "eval: substr: first argument must be a string", 0);
  }

  if(args->cdr == &NIL) return strexpr;

 posexpr = args->cdr->car->eval(env, args->cdr->car, 0);
  if(!IS_INT(posexpr)) {
    _free_expr(strexpr);
    _free_expr(posexpr);
    return _err_expr(0, "eval: substr: position argument must be an integer", 0);
  }
  pos = posexpr->intval;
  _free_expr(posexpr);

  len = strlen(strexpr->strval);
  if(pos > len) {
    _free_expr(strexpr);
    return _err_expr(0, "eval: substr: position argument larger than string length", 0);
  }

  if(args->cdr->cdr == &NIL) {
    len = len - pos;
  } else {
   lenexpr = args->cdr->cdr->car->eval(env, args->cdr->cdr->car, 0);
    if(!IS_INT(lenexpr)) {
      _free_expr(strexpr);
      _free_expr(lenexpr);
      return _err_expr(0, "eval: substr: length argument must be an integer", 0);
    }
    len = lenexpr->intval;
    _free_expr(lenexpr);

    if((pos+len) > strlen(strexpr->strval)) {
      _free_expr(strexpr);
      return _err_expr(0, "eval: substr: position+length arguments greater than string length", 0);
    }
  }

  char *str = calloc(len+1, sizeof(char));
  strncpy(str, strexpr->strval+pos, len);

  _free_expr(strexpr);
  return _str_expr(str);
}

expr_t *eval_op_strlen(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!ONE_ARGS(args)) {
    return _err_expr(0, "eval: strlen: takes exactly one argument", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0);
  
  if(!IS_STRING(e1)) {
    _free_expr(e1);
    return _err_expr(0, "eval: strlen: argument is not a string", 0);
  }

  expr_t *r = _int_expr(strlen(e1->strval));
  _free_expr(e1);
  return r;
}

expr_t *eval_op_split(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!TWO_ARGS(args)) {
    return _err_expr(0, "eval: split: takes exactly two argument", 0);
  }

  expr_t *strexpr = args->car->eval(env, args->car, 0);
  if(!IS_STRING(strexpr)) {
    _free_expr(strexpr);
    return _err_expr(0, "eval: split: first argument must be a string", 0);
  }

  expr_t *splitexpr = args->cdr->car->eval(env, args->cdr->car, 0);
  if(!IS_STRING(splitexpr)) {
    _free_expr(strexpr);
    _free_expr(splitexpr);
    return _err_expr(0, "eval: split: second argument must be a string", 0);
  }
  
  char *str = strexpr->strval, *split = splitexpr->strval;

  expr_t *r = &NIL, *tail;
  char *p = strtok(str, split);
  if(p) tail = (r = _list_expr(_str_expr(strdup(p)), &NIL));
  while((p = strtok(0, split))) {
    tail->cdr = _list_expr(_str_expr(strdup(p)), &NIL);
    tail = tail->cdr;
  }

  _free_expr(strexpr);
  _free_expr(splitexpr);
  return r;
}

expr_t *eval_op_concat(expr_t *env, const expr_t *self, const expr_t *args)
{
  char *s1 = calloc(1, sizeof(char));
  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *s2 = ptr->car->eval(env, ptr->car, 0);
    if(!IS_STRING(s2)) {
      _free_expr(s2);
      free(s1);
      return _err_expr(0, "eval: concat: all arguments must be strings", 0);
    }

    int l_s1 = strlen(s1);
    int l_s2 = strlen(s2->strval);
    char *tmp = calloc(l_s1 + l_s2 + 1, sizeof(char));
    strncat(tmp, s1, l_s1);
    strncat(tmp, s2->strval, l_s2);

    _free_expr(s2);
    free(s1);
    s1 = tmp;
  }

  // return empty string if we weren't passed any args
  return _str_expr(s1);
}

expr_t *eval_op_openif(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!ONE_ARGS(args)) {
    return _err_expr(0, "eval: openif: requires exactly 1 string argument", 0);
  }

 expr_t *e1 = args->car->eval(env, args->car, 0);
  if(!IS_STRING(e1)) {
    _free_expr(e1);
    return _err_expr(0, "eval: openif: requires exactly 1 string argument", 0);
  }

  FILE *fp = fopen(e1->strval, "r");
  if(!fp) {
    _free_expr(e1);
    return _err_expr(0, "eval: openif: error while opening file: %s", strerror(errno));
  }

  _free_expr(e1);
  return _port_expr(fp);
}

expr_t *eval_op_closeif(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *e1 = args->car->eval(env, args->car, 0), *r;
  if(!IS_PORT(e1) || args->cdr != &NIL) {
    r = _err_expr(e1, "eval: closeif: requires exactly 1 port argument", 0);
  } else {
    fclose(e1->fp);
    r = &NIL;
  }

  _free_expr(e1);
  return r;
}

expr_t *eval_op_readline(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!ONE_ARGS(args)) {
    return _err_expr(0, "eval: readline: requires exactly 1 port argument", 0);
  }

  expr_t *e1 = args->car->eval(env, args->car, 0);
  if(!IS_PORT(e1)) {
    _free_expr(e1);
    return _err_expr(0, "eval: readline: requires exactly 1 port argument", 0);
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

expr_t *eval_op_env(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(args == &NIL) return _clone_expr(env->car);
}

expr_t *eval_op_keys(expr_t *env, const expr_t *self, const expr_t *args)
{
  expr_t *r = keys(env);
  return r;
}

expr_t *eval_op_time(expr_t *env, const expr_t *self, const expr_t *args)
{
  time_t t0 = time(0);

  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
   expr_t *e1 = ptr->car->eval(env, ptr->car, 0);
    _free_expr(e1);
  }

  time_t t1 = time(0);
  double timediff = difftime(t1, t0);
  char *buf = calloc(_DEFAULT_STR_SIZE, sizeof(char));
  sprintf(buf, "time: all operations completed in %f seconds", timediff);

  return _str_expr(buf);
}

expr_t *eval_op_sleep(expr_t *env, const expr_t *self, const expr_t *args)
{
  for(const expr_t *ptr = args; ptr != &NIL; ptr = ptr->cdr) {
    expr_t *t = ptr->car->eval(env, ptr->car, 0);
    if(!IS_INT(t)) {
      _free_expr(t);
      return _err_expr(0, "eval: sleep: all arguments must be integers", 0);
    }
    sleep(t->intval);
    _free_expr(t);
  }

  return &NIL;
}

expr_t *eval_re(expr_t *env, const expr_t *self, const expr_t *args)
{
  if(!args) return _clone_expr(self);
  if(!ONE_ARGS(args)) return _err_expr(0, "eval: re: regex takes exactly one argument", 0);

  expr_t *e = args->car->eval(env, args->car, 0);
  if(!IS_STRING(e)) {
    _free_expr(e);
    return _err_expr(0, "eval: re: regex takes a string argument", 0);
  }

  char *restr = strdup(self->strval+3), *p;
  int cflags = 0;
  for(p = restr; *p != '/'; p++);
  *p = 0;
  for(p = p+1; *p; p++) {
    switch(*p) {
    case 'e': cflags |= REG_EXTENDED; break; // extended regex
    case 'i': cflags |= REG_ICASE; break; // case insensitivity
    default: {
      _free_expr(e);
      free(restr);
      return _err_expr(0, "eval: re: unsupported flag(s)", 0);
    }
    }
  }
		       
  regex_t re;
  if(regcomp(&re, restr, cflags) != 0) { // TODO support flags
    _free_expr(e);
    free(restr);
    return _err_expr(0, "eval: re: unable to compile regex", 0);
  }

  regmatch_t m[_MAX_RE_MATCHES];
  int reti = regexec(&re, e->strval, _MAX_RE_MATCHES, m, 0); // TODO support flags?

  expr_t *r = &NIL, *tmp;
  if(reti == 0) {
    for(int i = _MAX_RE_MATCHES-1; i >= 0; i--) { // TODO inefficient...
      if(m[i].rm_so != -1) {
	char *str = strndup(e->strval+m[i].rm_so, m[i].rm_eo-m[i].rm_so);
	tmp = _list_expr(_str_expr(str), r);
	r = tmp;
      }
    }
  } else if(reti != REG_NOMATCH) {
    char errmsg[_DEFAULT_STR_SIZE];
    regerror(reti, &re, errmsg, _DEFAULT_STR_SIZE);
    r = _err_expr(0, "eval: re: match error", errmsg);
  } // otherwise it's just no match, so we return NIL

  _free_expr(e);
  free(restr);
  regfree(&re);
  return r;
}
