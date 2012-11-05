#include "punky.h"
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
static expr_t *define_variable(env_t *env, char *id, expr_t *e);

// TODO get rid of this
void _print(FILE *out, expr_t *e, int indent, int depth);

expr_t *eval_idem(env_t *env, expr_t *e)
{
  return _clone_expr(e);
}

expr_t *eval_op_define(env_t *env, expr_t *e)
{
  // TODO error checking!
  expr_t *id_expr = e->car, *value = e->cdr->car->eval(env, e->cdr->car);
  return define_variable(env, id_expr->strval, value);
}

expr_t *eval_op_lambda(env_t *env, expr_t *e)
{
  return _list_expr(_op_expr(strdup("lambda"), &eval_op_lambda), _clone_expr(e));
}

expr_t *eval_function_call(env_t *env, expr_t *fn, expr_t *args)
{
  expr_t *formals = fn->cdr->car, *body = fn->cdr->cdr->car, *f_ptr, *a_ptr, *result;

  // fprintf(stdout, "formals: "); _print(stdout, formals, 0, 0);
  // fprintf(stdout, "body: "); _print(stdout, body, 0, 0);
  // fprintf(stdout, "args: "); _print(stdout, args, 0, 0);

  env_t funenv;
  init_env(&funenv, env);
  
  // evaluate each arg and bind it to its formal parameter in the new env we've created
  for(f_ptr = formals, a_ptr = args; 
      ((f_ptr != &NIL) && (a_ptr != &NIL)); 
      f_ptr = f_ptr->cdr, a_ptr = a_ptr->cdr)
    {
      expr_t *tmp = define_variable(&funenv, f_ptr->car->strval, a_ptr->car->eval(env, a_ptr->car));
      _free_expr(tmp); // we don't need the id
    }

  // make sure we've provided the right number of arguments
  if((f_ptr != &NIL) || (a_ptr != &NIL)) {
    result = _error("incorrect number of arguments");
  } else {
    result = body->eval(&funenv, body);
  }

  // clean up and return
  free_env(&funenv);
  return result;
}

expr_t *eval_list(env_t *env, expr_t *e)
{
  // we already know how to execute ops
  if(e->car->type == OP_T) 
    return e->car->eval(env, e->cdr);

  // since we don't know how to execute other kinds of things
  expr_t *fn = e->car->eval(env, e->car);
  expr_t *result = eval_function_call(env, fn, e->cdr);
  _free_expr(fn);
  return result;
}

expr_t *eval_ident(env_t *env, expr_t *e)
{
  expr_t *result = get(env, e->strval);
  return result ? result : _error("unbound variable");
}

expr_t *eval_op_add(env_t *env, expr_t *e)
{
  return eval_op_add_int(env, e, 0);
}

expr_t *eval_op_sub(env_t *env, expr_t *e)
{
  if(e == &NIL) return _int_expr(0);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)e1->floatval); break;
  default: result = _error("attempt to subtract a non-numeric value");
  }
  
  // if we only had one operand, we want to return the negative of it
  if(e->cdr == &NIL) {
    switch(e1->type) {
    case INTEGER_T: result->intval = -result->intval; break;
    case FLOAT_T: result->floatval = -result->intval; break;
    }
  }

  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

expr_t *eval_op_mul(env_t *env, expr_t *e)
{
  return eval_op_mul_int(env, e, 1);
}

expr_t *eval_op_div(env_t *env, expr_t *e)
{
  if(e == &NIL) return _error("need at least two arguments for division");
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // if we only had one operand, that's an error!
  if(e->cdr == &NIL) result = _error("need at least two arguments for division");

  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}
expr_t *eval_op_car(env_t *env, expr_t *e)
{
  expr_t *l = e->car->eval(env, e->car), *car = _clone_expr(l->car);
  _free_expr(l);
  return car;
}

expr_t *eval_op_cdr(env_t *env, expr_t *e)
{
  expr_t *l = e->car->eval(env, e->car), *cdr = _clone_expr(l->cdr);
  _free_expr(l);
  return cdr;
}

expr_t *eval_op_cons(env_t *env, expr_t *e)
{
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

expr_t *eval_op_quote(env_t *env, expr_t *e)
{
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
    expr_t *tmp = define_variable(&letenv, d_ptr->car->car->strval, d_ptr->car->cdr->car->eval(env, d_ptr->car->cdr->car));
    _free_expr(tmp); // we don't need the id
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
    _free_expr(cond); 
    return _error("if: boolean value expected");
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
    _free_expr(b);
    return _error("not: boolean value expected");
  }
  return (b == &T) ? &F : &T;
}

expr_t *eval_op_and(env_t *env, expr_t *e)
{
  expr_t *b1 = e->car->eval(env, e->car);
  if(b1->type != BOOL_T) {
    _free_expr(b1);
    return _error("and: boolean value expected");
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    _free_expr(b2);
    return _error("and: boolean value expected");
  }

  return ((b1 == &T) && (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_or(env_t *env, expr_t *e)
{
  expr_t *b1 = e->car->eval(env, e->car);
  if(b1->type != BOOL_T) {
    _free_expr(b1);
    return _error("or: boolean value expected");
  }
  expr_t *b2 = e->cdr->car->eval(env, e->cdr->car);
  if(b2->type != BOOL_T) {
    _free_expr(b2);
    return _error("or: boolean value expected");
  }

  return ((b1 == &T) || (b2 == &T)) ? &T : &F;
}

expr_t *eval_op_equal(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  return (compare(e1, e2) == 0) ? &T : &F;
}

expr_t *eval_op_lt(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  return (compare(e1, e2) < 0) ? &T : &F;
}

expr_t *eval_op_gt(env_t *env, expr_t *e)
{
  expr_t *e1 = e->car->eval(env, e->car), *e2 = e->cdr->car->eval(env, e->cdr->car);
  return (compare(e1, e2) > 0) ? &T : &F;
}


static expr_t *eval_op_add_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to add a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_add_float(env, e->cdr, partial + (float)e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, partial + e1->floatval); break;
  default: result = _error("attempt to add to a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_add_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to add a non-numeric value"); // TODO this error message isn't quite sensible

  // we'll use this to hold the result
  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_add_int(env, e->cdr, partial + e1->intval); break;
  case FLOAT_T: result = eval_op_add_float(env, e->cdr, (float)partial + e1->floatval); break;
  default: result = _error("attempt to add a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_sub_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_float(env, e->cdr, partial - (float)e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, partial - e1->floatval); break;
  default: result = _error("attempt to subtract a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_sub_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_sub_int(env, e->cdr, partial - e1->intval); break;
  case FLOAT_T: result = eval_op_sub_float(env, e->cdr, (float)partial - e1->floatval); break;
  default: result = _error("attempt to subtract a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_mul_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to multiply a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_mul_float(env, e->cdr, partial * (float)e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, partial * e1->floatval); break;
  default: result = _error("attempt to multiply a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_mul_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to multiply a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_mul_int(env, e->cdr, partial * e1->intval); break;
  case FLOAT_T: result = eval_op_mul_float(env, e->cdr, (float)partial * e1->floatval); break;
  default: result = _error("attempt to multiply a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_div_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_float(env, e->cdr, partial / (float)e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, partial / e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_op_div_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = e->car->eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_op_div_int(env, e->cdr, partial / e1->intval); break;
  case FLOAT_T: result = eval_op_div_float(env, e->cdr, (float)partial / e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *define_variable(env_t *env, char *id, expr_t *e)
{
  // fprintf(stderr, "defining: %s->", id); _print(stderr, e, 0, 0);
  return put(env, id, e);
}

punky_t *eval(punky_t *p)
{
  expr_t *e = p->e->eval(&p->env, p->e); // _eval(&p->env, p->e);
  _free_expr(p->e); // free up whatever we parsed
  p->e = e;
  return p;
}
