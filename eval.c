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

expr_t *eval_idem(env_t *env, expr_t *e)
{
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
  
  // and the value
  expr_t *value = e->cdr->car->eval(env, e->cdr->car);
  if(!value) {
    // fprintf(stderr, "eval: error: define: failed to define variable");
    // we'll assume evaluating e->cdr->car consumed that part of e, so
    // we need only free the rest
    _free_expr(e->car);
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  if(!e->cdr->ref) free(e->cdr); 
  if(!e->ref) free(e);
  put(env, id, value);
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
  expr_t *formals = fn->cdr->car, *body = fn->cdr->cdr->car, *f_ptr = formals, *a_ptr = args, *tmp, *result;

  if(!fn->cdr->cdr->ref) free(fn->cdr->cdr);
  if(!fn->cdr->ref) free(fn->cdr);

  env_t funenv;
  init_env(&funenv, env);
  
  // evaluate each arg and bind it to its formal parameter in the new env we've created
  // for(f_ptr = formals, a_ptr = args; 
  while((f_ptr != &NIL) && (a_ptr != &NIL)) {
    put(&funenv, f_ptr->car, a_ptr->car->eval(env, a_ptr->car));

    tmp = f_ptr->cdr;
    if(!f_ptr->ref) free(f_ptr);
    f_ptr = tmp;

    tmp = a_ptr->cdr;
    if(!a_ptr->ref) free(a_ptr);
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

  // make sure we don't lose our shit when we free up the temporary env
  _inc_ref(result); // _set_ref(result, result->ref+1);
  // clean up and return
  free_env(&funenv);
  // get rid of the ref count so we can free this up after the fact
  _dec_ref(result); // _set_ref(result, result->ref-1);

  _free_expr(fn->car);
  if(!fn->ref) free(fn);
  //  if(!args->ref) free(args);
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
  expr_t *result = get(env, e);
  if(!result) fprintf(stderr, "eval: error: unbound variable\n");
  //  _free_expr(e);
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
  case INT_T: result = eval_op_add_float(env, e->cdr, partial + (float)e1->intval); break;
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
  case INT_T: result = eval_op_add_int(env, e->cdr, partial + e1->intval); break;
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
  case INT_T: result = eval_op_sub_float(env, e->cdr, partial - (float)e1->intval); break;
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
  case INT_T: result = eval_op_sub_int(env, e->cdr, partial - e1->intval); break;
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
  case INT_T: result = eval_op_sub_int(env, e->cdr, e1->intval); break;
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
    case INT_T: result->intval = -result->intval; break;
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
  case INT_T: result = eval_op_mul_float(env, e->cdr, partial * (float)e1->intval); break;
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
  case INT_T: result = eval_op_mul_int(env, e->cdr, partial * e1->intval); break;
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
  case INT_T: result = eval_op_div_float(env, e->cdr, partial / (float)e1->intval); break;
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
  case INT_T: result = eval_op_div_int(env, e->cdr, partial / e1->intval); break;
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
  case INT_T: result = eval_op_div_int(env, e->cdr, e1->intval); break;
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
  expr_t *e1 = e->car->eval(env, e->car), *result;

  if(!IS_LIST(e1)) {
    fprintf(stderr, "eval: error: car: requires list argument\n");
    return 0;
  }

  result = e1->car;

  _free_expr(e1->cdr);
  if(!e1->ref) free(e1);
  if(!e->ref) free(e);

  return result;
}

expr_t *eval_op_cdr(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *result;

  if(!IS_LIST(e1)) {
    fprintf(stderr, "eval: error: car: requires list argument\n");
    return 0;
  }

  result = e1->cdr;

  _free_expr(e1->car);
  if(!e1->ref) free(e1);
  if(!e->ref) free(e);

  return result;
}

expr_t *eval_op_cons(env_t *env, expr_t *e)
{
  if(!TWO_ARGS(e)) {
    fprintf(stderr, "eval: error: cons: requires exactly 2 arguments\n");
    return 0;
  }
  expr_t *e1 = e->car->eval(env, e->car);
  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);

  expr_t *result = _list_expr(e1, e2);

  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);

  return result;
}

expr_t *eval_op_list(env_t *env, expr_t *e)
{
  if(e == &NIL) return &NIL;

  expr_t *car = e->car->eval(env, e->car);
  if(!car) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *cdr = eval_op_list(env, e->cdr);
  if(!cdr) {
    if(!e->ref) free(e);
    return 0;
  }
  
  if(!e->ref) free(e);
  return _list_expr(car, cdr);
}

expr_t *eval_op_append(env_t *env, expr_t *e)
{
  // TODO make append accept any number of args
  if(!(TWO_ARGS(e))) { 
    fprintf(stderr, "eval: error: append: incorrect number of arguments"); 
    _free_expr(e);
    return 0; 
  }

  // TODO null check!
  expr_t *l1 = e->car->eval(env, e->car);
  expr_t *l2 = e->cdr->car->eval(env, e->cdr->car);

  if(l1 == &NIL) return l2;

  expr_t *ptr;
  for(ptr = l1; ptr->cdr != &NIL; ptr = ptr->cdr){}

  ptr->cdr = l2;
  
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return l1;
}

expr_t *eval_op_quote(env_t *env, expr_t *e)
{
  if(!(ONE_ARGS(e))) { 
    fprintf(stderr, "eval: error: quote: incorrect number of arguments"); 
    _free_expr(e);
    return 0; 
  }
  
  expr_t *result = e->car;
  if(!e->ref) free(e);

  return result;
}

expr_t *eval_op_let(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: let: incorrect number of arguments"); 
    _free_expr(e);
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
      if(!def->car->ref) free(def->car);
      if(!def->cdr->ref) free(def->cdr);
      if(!def->ref) free(def);
      if(!d_ptr->ref) free(d_ptr);
      _free_expr(d_ptr->cdr);
      _free_expr(body);
      if(!defs->ref) free(defs);
      if(!e->cdr->ref) free(e->cdr);
      if(!e->ref) free(e);
      return 0;
    }

    put(&letenv, def->car, value);
    if(!def->car->ref) free(def->car);
    if(!def->cdr->ref) free(def->cdr);
    if(!def->ref) free(def);

    tmp = d_ptr->cdr;
    if(!d_ptr->ref) free(d_ptr);
    d_ptr = tmp;
  }

  result = body->eval(&letenv, body);

  free_env(&letenv);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_if(env_t *env, expr_t *e)
{
  // TODO test for either two or three args
  expr_t *cond = e->car->eval(env, e->car);
  if(!cond) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result;
  if(cond->type != BOOL_T) {
    fprintf(stderr, "eval: error: if: boolean value expected\n");
    _free_expr(e->cdr);
    result = 0;
  } else if(cond == &T) {
    result = e->cdr->car->eval(env, e->cdr->car);
    _free_expr(e->cdr->cdr);
    if(!e->cdr->ref) free(e->cdr);
  } else if(e->cdr->cdr != &NIL) { // cond == &F, else
    result = e->cdr->cdr->car->eval(env, e->cdr->cdr->car);
    _free_expr(e->cdr->car);
    if(!e->cdr->cdr->ref) free(e->cdr->cdr);
    if(!e->cdr->ref) free(e->cdr);
  } else { // cond == &F, no "else"
    result = &NIL;
    _free_expr(e->cdr);
  }

  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_not(env_t *env, expr_t *e)
{
  if(!(ONE_ARGS(e))) {
    fprintf(stderr, "eval: error: not: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }
  
  expr_t *b = e->car->eval(env, e->car), *result;
  if(b->type != BOOL_T) {
    _free_expr(b);
    fprintf(stderr, "not: boolean value expected");
    result = 0;
  } else {
    result = (b == &T) ? &F : &T;
  }
  
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_and(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: and: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *b1 = e->car->eval(env, e->car), *result;
  if(b1->type != BOOL_T) {
    _free_expr(b1);
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    fprintf(stderr, "and: boolean value expected");
    return 0;
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    _free_expr(b2);
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    fprintf(stderr, "and: boolean value expected");
    return 0;
  }

  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return ((b1 == &T) && (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_or(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: or: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *b1 = e->car->eval(env, e->car), *result;
  if(b1->type != BOOL_T) {
    _free_expr(b1);
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    fprintf(stderr, "or: boolean value expected");
    return 0;
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    _free_expr(b2);
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    fprintf(stderr, "or: boolean value expected");
    return 0;
  }

  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return ((b1 == &T) || (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_equal(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: equal: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!e1) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(!e2) {
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result = (compare(e1, e2) == 0) ? &T : &F;

  _free_expr(e1); _free_expr(e2);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_lt(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: lt: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!e1) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(!e2) {
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result = (compare(e1, e2) < 0) ? &T : &F;

  _free_expr(e1); _free_expr(e2);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_gt(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: gt: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!e1) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(!e2) {
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result = (compare(e1, e2) > 0) ? &T : &F;

  _free_expr(e1); _free_expr(e2);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_le(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: le: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!e1) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(!e2) {
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result = (compare(e1, e2) <= 0) ? &T : &F;

  _free_expr(e1); _free_expr(e2);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_ge(env_t *env, expr_t *e)
{
  if(!(TWO_ARGS(e))) {
    fprintf(stderr, "eval: error: ge: incorrect number of arguments");
    _free_expr(e);
    return 0;
  }

  expr_t *e1 = e->car->eval(env, e->car);
  if(!e1) {
    _free_expr(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *e2 = e->cdr->car->eval(env, e->cdr->car);
  if(!e2) {
    if(!e->cdr->ref) free(e->cdr);
    if(!e->ref) free(e);
    return 0;
  }

  expr_t *result = (compare(e1, e2) >= 0) ? &T : &F;

  _free_expr(e1); _free_expr(e2);
  if(!e->cdr->ref) free(e->cdr);
  if(!e->ref) free(e);
  return result;
}

expr_t *eval_op_substr(env_t *env, expr_t *e)
{
  if(NO_ARGS(e)) {
    fprintf(stderr, "eval: error: substr: requires between 1 and 3 args\n");
    return 0;
  }

  expr_t *strexpr = e->car->eval(env, e->car), *cdr = e->cdr;
  if(!e->ref) free(e);

  if(!IS_STRING(strexpr)) {
    fprintf(stderr, "eval: error: substr: first argument must be a string\n");
    return 0;
  }
  
  if(cdr == &NIL) {
    return strexpr;
  }

  char *str = strexpr->strval;

  expr_t *posexpr = cdr->car->eval(env, cdr->car);
  if(!IS_INT(posexpr)) {
    fprintf(stderr, "eval: error: substr: position argument must be an int\n");
    return 0;
  }

  int pos = posexpr->intval, len = strlen(str) - pos;
  if(pos >= strlen(str)) {
    fprintf(stderr, "eval: error: substr: position must be <= string lengtrh\n");
    return 0;
  }

  if(cdr->cdr != &NIL) {
    expr_t *lenexpr = cdr->cdr->car->eval(env, cdr->cdr->car);
    if(!IS_INT(lenexpr)) {
      fprintf(stderr, "eval: error: substr: length argument must be an int\n");
      return 0;
    }
    len = lenexpr->intval;
  }

  char *r = calloc(len+1, sizeof(char));
  strncpy(r, str+pos, len);
  
  return _str_expr(r);
}

expr_t *eval_op_strlen(env_t *env, expr_t *e)
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

  return _int_expr(strlen(e1->strval));
}

punky_t *punky_eval(punky_t *p)
{
  expr_t *e = p->e->eval(&p->env, p->e); // _eval(&p->env, p->e);
  // _free_expr(p->e, e); // free up whatever we parsed
  p->e = e;
  return p;
}
