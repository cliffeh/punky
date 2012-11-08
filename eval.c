#include "punky.h"
#include "types.h"
#include "env.h"
#include "eval.h"

/* static convenience functions */
static expr_t *eval_op_add_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_op_add_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_op_sub_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_op_sub_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_op_mul_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_op_mul_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_op_div_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_op_div_int(env_t *env, expr_t *e, int partial);

// TODO get rid of this
void _print(FILE *out, expr_t *e, int indent, int depth);

expr_t *eval_idem(env_t *env, expr_t *e)
{
  return e;
}

expr_t *eval_clone(env_t *env, expr_t *e)
{
  //   return _clone_expr(e);
   return e;
}

expr_t *eval_op_define(env_t *env, expr_t *e)
{
  // we are expecting two arguments:
  if(!(TWO_ARGS(e))) { 
    fprintf(stderr, "eval: error: define: incorrect number of arguments\n");
    _free_expr(e);
    return 0; 
  }

  // an identifier
  expr_t *id = e->car;
  if(!IS_IDENT(id)) {
    fprintf(stderr, "eval: error: define: invalid identifier\n");
    _free_expr(e);
    return 0;
  }
  
  // and the value
  expr_t *value = e->cdr->car->eval(env, e->cdr->car);
  if(!value) {
    fprintf(stderr, "eval: error: define: failed to define variable '%s'\n", id->strval);
    // we'll assume evaluating e->cdr->car consumed that part of e, so
    // we need only free the rest
    _free_expr(e->car);
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  if(!e->cdr->ref) free(e->cdr); 
  if(!e->ref) free(e);
  put(env, strdup(id->strval), value);
  return id;
}

expr_t *eval_op_lambda(env_t *env, expr_t *e)
{
  // TODO this is clunky as shit...
  return _list_expr(_op_expr(strdup("lambda"), &eval_op_lambda), e);
}

expr_t *eval_function_call(env_t *env, expr_t *fn, expr_t *args)
{
  if(!(THREE_ARGS(fn))) { fprintf(stderr, "eval: error: invalid function call"); return 0; }
  expr_t *formals = fn->cdr->car, *body = fn->cdr->cdr->car, *f_ptr, *a_ptr, *result;

  env_t funenv;
  init_env(&funenv, env);
  
  // evaluate each arg and bind it to its formal parameter in the new env we've created
  for(f_ptr = formals, a_ptr = args; 
      ((f_ptr != &NIL) && (a_ptr != &NIL)); 
      f_ptr = f_ptr->cdr, a_ptr = a_ptr->cdr)
    {
      put(&funenv, strdup(f_ptr->car->strval), a_ptr->car->eval(env, a_ptr->car));
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

expr_t *eval_list(env_t *env, expr_t *e)
{
  expr_t *result;
  if(e->car->type == OP_T) {
    // we already know how to execute ops
    result = e->car->eval(env, e->cdr);
    _free_expr(e->car);
  } else {
    // since we don't know how to execute other kinds of things
    expr_t *fn = e->car->eval(env, e->car);
    result = eval_function_call(env, fn, e->cdr);
  }
  
  // cleanup
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_ident(env_t *env, expr_t *e)
{
  expr_t *result = get(env, e->strval);
  if(!result) fprintf(stderr, "eval: error: unbound variable '%s'\n", e->strval);
  _free_expr(e);
  return result;
}

static expr_t *eval_op_add_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "eval: add: unexpected argument type\n");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_add_float(env, e->cdr, partial + (float)e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, partial + e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to add to a non-numeric value");
    _free_expr(e->cdr);
    result = 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

static expr_t *eval_op_add_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    fprintf(stderr, "eval: add: unexpected argument type\n");
    _free_expr(e);
    return 0; 
  }

  // we'll use this to hold the result
  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_add_int(env, e->cdr, partial + e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, (float)partial + e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to add a non-numeric value");
    _free_expr(e->cdr);
    result = 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_add(env_t *env, expr_t *e)
{
  return eval_op_add_int(env, e, 0);
}

static expr_t *eval_op_sub_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to subtract a non-numeric value");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_float(env, e->cdr, partial - (float)e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, partial - e1->floatval); break;
  default: { 
    fprintf(stderr, "attempt to subtract a non-numeric value");
    _free_expr(e->cdr);
    result = 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

static expr_t *eval_op_sub_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) {
    fprintf(stderr, "attempt to subtract a non-numeric value"); 
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_int(env, e->cdr, partial - e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)partial - e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to subtract a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_sub(env_t *env, expr_t *e)
{
  if(NO_ARGS(e)) return _int_expr(0);
  if(!(IS_LIST(e))) {
    fprintf(stderr, "eval: error: attempt to subtract a non-numeric value"); 
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    fprintf(stderr, "eval: error: attempt to subtract a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  // if we only had one operand, we want to return the negative of it
  if(e->cdr == &NIL) {
    switch(e1->type) {
    case INTEGER_T: result->intval = -result->intval; break;
    case FLOAT_T: result->floatval = -result->intval; break;
    }
  }

  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

static expr_t *eval_op_mul_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to multiply a non-numeric value"); 
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_mul_float(env, e->cdr, partial * (float)e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, partial * e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to multiply a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

static expr_t *eval_op_mul_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to multiply a non-numeric value"); 
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_mul_int(env, e->cdr, partial * e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, (float)partial * e1->floatval); break;
  default: { 
    fprintf(stderr, "attempt to multiply a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_mul(env_t *env, expr_t *e)
{
  return eval_op_mul_int(env, e, 1);
}

static expr_t *eval_op_div_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to divide a non-numeric value"); 
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_float(env, e->cdr, partial / (float)e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, partial / e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to divide a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

static expr_t *eval_op_div_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) { 
    fprintf(stderr, "attempt to divide a non-numeric value"); 
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_int(env, e->cdr, partial / e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)partial / e1->floatval); break;
  default: {
    fprintf(stderr, "attempt to divide a non-numeric value");
    _free_expr(e->cdr);
    return 0;
  }
  }
  
  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_div(env_t *env, expr_t *e)
{
  // this is a bit inefficient, but makes things easy to read
  if(NO_ARGS(e)) { 
    fprintf(stderr, "eval: error: div: need at least two arguments for division\n"); 
    _free_expr(e);
    return 0; 
  }
  if(ONE_ARGS(e)) { 
    fprintf(stderr, "eval: error: div: need at least two arguments for division\n");
    _free_expr(e);
    return 0; 
  }

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)e1->floatval); break;
  default: {
    fprintf(stderr, "eval: error: div: attempt to divide a non-numeric value\n");
    _free_expr(e->cdr);
    return 0;
  }
  }

  _free_expr(e1);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_car(env_t *env, expr_t *e)
{
  if(!(ONE_ARGS(e))) { fprintf(stderr, "eval: error: car: incorrect number of arguments\n"); return 0; }
  expr_t *l = e->car->eval(env, e->car);
  if(!(IS_LIST(l))) { fprintf(stderr, "eval: error: car: attempted on non-list\n"); return 0; }
  expr_t *car = _clone_expr(l->car);
  // _free_expr(l);
  return car;
}

expr_t *eval_op_cdr(env_t *env, expr_t *e)
{
  if(!(ONE_ARGS(e))) { fprintf(stderr, "eval: error: cdr: incorrect number of arguments\n"); return 0; }
  expr_t *l = e->car->eval(env, e->car);
  if(!(IS_LIST(l))) { fprintf(stderr, "eval: error: cdr: attempted on non-list\n"); return 0; }
  expr_t *cdr = _clone_expr(l->cdr);
  // _free_expr(l);
  return cdr;
}

expr_t *eval_op_cons(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) { fprintf(stderr, "eval: error: cons: incorrect number of arguments"); return 0; }
  return _list_expr(e->car->eval(env, e->car), e->cdr->car->eval(env, e->cdr->car));
}

expr_t *eval_op_list(env_t *env, expr_t *e)
{
  if(e == &NIL) return &NIL;
  return _list_expr(e->car->eval(env, e->car), eval_op_list(env, e->cdr));
  /*
  expr_t *result = _list_expr(e->car->eval(env, e->car), &NIL), *e_ptr, *r_ptr;
  for(e_ptr = e->cdr, r_ptr = result; 
      e_ptr != &NIL; 
      e_ptr = e_ptr->cdr, r_ptr = r_ptr->cdr)
    {
      r_ptr->cdr = _list_expr(e_ptr->car->eval(env, e_ptr->car), &NIL);
    }
  return result;
  */
}

expr_t *eval_op_append(env_t *env, expr_t *e)
{
  expr_t *l1 = e->car->eval(env, e->car), *l2 = e->cdr->car->eval(env, e->cdr->car), *ptr, *result;
  if(l1 == &NIL) return l2;
  for(ptr = l1; ptr->cdr != &NIL; ptr = ptr->cdr);
  ptr->cdr = l2;
  result = _clone_expr(l1);
  // _free_expr(l1);
  return result;
}

expr_t *eval_op_quote(env_t *env, expr_t *e)
{
  if(!(ONE_ARGS(e))) { fprintf(stderr, "eval: error: quote: incorrect number of arguments"); return 0; }
  return _clone_expr(e->car);
}

expr_t *eval_op_let(env_t *env, expr_t *e)
{
  expr_t *defs = e->car, *body = e->cdr->car, *d_ptr, *result;

  // fprintf(stderr, "defs: "); _print(stderr, defs, 0, 0);
  // fprintf(stderr, "body: "); _print(stderr, body, 0, 0);

  env_t letenv;
  init_env(&letenv, env);

  for(d_ptr = defs; d_ptr != &NIL; d_ptr = d_ptr->cdr) {
    put(&letenv, d_ptr->car->car->strval, d_ptr->car->cdr->car->eval(env, d_ptr->car->cdr->car));
  }

  result = body->eval(&letenv, body);
  // fprintf(stderr, "result: "); _print(stderr, result, 0, 0);

  free_env(&letenv);
  return result;
}

expr_t *eval_op_if(env_t *env, expr_t *e)
{
  expr_t *cond = e->car->eval(env, e->car);
  if(cond->type != BOOL_T) {
    // _free_expr(cond);
    fprintf(stderr, "eval: error: if: boolean value expected\n");
  } else if(cond == &T) {
    return e->cdr->car->eval(env, e->cdr->car);
  } else if(e->cdr->cdr != &NIL) {
    return e->cdr->cdr->car->eval(env, e->cdr->cdr->car);
  } else {
    return &NIL;
  }
}

expr_t *eval_op_not(env_t *env, expr_t *e)
{
  expr_t *b = e->car->eval(env, e->car);
  if(b->type != BOOL_T) {
    // _free_expr(b, 0);
    fprintf(stderr, "not: boolean value expected");
    return 0;
  }
  return (b == &T) ? &F : &T;
}

expr_t *eval_op_and(env_t *env, expr_t *e)
{
  expr_t *b1 = e->car->eval(env, e->car);
  if(b1->type != BOOL_T) {
    // fprintf(stderr, "and: b1: error: "); _print(stderr, b1, 0, 0);
    // _free_expr(b1, 0);
    fprintf(stderr, "and: boolean value expected");
    return 0;
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    // fprintf(stderr, "and: b2: error: "); _print(stderr, b2, 0, 0);
    // _free_expr(b2, 0);
    fprintf(stderr, "and: boolean value expected");
    return 0;
  }

  return ((b1 == &T) && (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_or(env_t *env, expr_t *e)
{
  expr_t *b1 = e->car->eval(env, e->car);
  if(b1->type != BOOL_T) {
    // _free_expr(b1, 0);
    fprintf(stderr, "or: boolean value expected");
    return 0;
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    // _free_expr(b2, 0);
    fprintf(stderr, "or: boolean value expected");
    return 0;
  }

  return ((b1 == &T) || (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_equal(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  expr_t *result = (compare(e1, e2) == 0) ? &T : &F;
  // _free_expr(e1, result); // _free_expr(e2, result);
  return result;
}

expr_t *eval_op_lt(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  expr_t *result = (compare(e1, e2) < 0) ? &T : &F;
  // _free_expr(e1, result); // _free_expr(e2, result);
  return result;
}

expr_t *eval_op_gt(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  expr_t *result = (compare(e1, e2) > 0) ? &T : &F;
  // _free_expr(e1, result); // _free_expr(e2, result);
  return result;
}

expr_t *eval_op_le(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  expr_t *result = (compare(e1, e2) <= 0) ? &T : &F;
  // _free_expr(e1, result); // _free_expr(e2, result);
  return result;
}

expr_t *eval_op_ge(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  expr_t *result = (compare(e1, e2) >= 0) ? &T : &F;
  // _free_expr(e1, result); // _free_expr(e2, result);
  return result;
}

punky_t *eval(punky_t *p)
{
  expr_t *e = p->e->eval(&p->env, p->e); // _eval(&p->env, p->e);
  // _free_expr(p->e, e); // free up whatever we parsed
  p->e = e;
  return p;
}
