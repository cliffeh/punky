#include "punky.h"
#include "env.h"
#include "eval.h"

static expr_t *_eval(env_t *env, expr_t *e);
static expr_t *eval_add_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_add_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_add(env_t *env, expr_t *e);
static expr_t *eval_sub_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_sub_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_sub(env_t *env, expr_t *e);
static expr_t *eval_mul_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_mul_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_mul(env_t *env, expr_t *e);
static expr_t *eval_div_float(env_t *env, expr_t *e, float partial);
static expr_t *eval_div_int(env_t *env, expr_t *e, int partial);
static expr_t *eval_div(env_t *env, expr_t *e);

expr_t *eval_args(env_t *env, expr_t *e, int count)
{
  if((count == 0) && (e == &NIL)) return e;
  if(e->type != LIST_T) return _error("expected a list of arguments");

  int i;
  expr_t *result = _list_expr(_eval(env, e->car), &NIL), *r_ptr, *e_ptr;
  for(i = 1, r_ptr = result, e_ptr = e->cdr; e_ptr->type == LIST_T; i++, r_ptr = r_ptr->cdr, e_ptr = e_ptr->cdr) {
    r_ptr->cdr = _list_expr(e_ptr->car, &NIL);
  }
  if(i != count) {
    _free_expr(result);
    return _error("incorrect number of arguments");
  }
  return result;
}

static expr_t *eval_add_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to add a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_add_float(env, e->cdr, partial + (float)e1->intval); break;
  case FLOAT_T: result = eval_add_float(env, e->cdr, partial + e1->floatval); break;
  default: result = _error("attempt to add to a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_add_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to add a non-numeric value"); // TODO this error message isn't quite sensible

  // we'll use this to hold the result
  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_add_int(env, e->cdr, partial + e1->intval); break;
  case FLOAT_T: result = eval_add_float(env, e->cdr, (float)partial + e1->floatval); break;
  default: result = _error("attempt to add a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_add(env_t *env, expr_t *e)
{
  return eval_add_int(env, e, 0);
}

static expr_t *eval_sub_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_sub_int(env, e->cdr, partial - (float)e1->intval); break;
  case FLOAT_T: result = eval_sub_float(env, e->cdr, partial - e1->floatval); break;
  default: result = _error("attempt to subtract a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_sub_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_sub_int(env, e->cdr, partial - e1->intval); break;
  case FLOAT_T: result = eval_sub_float(env, e->cdr, (float)partial - e1->floatval); break;
  default: result = _error("attempt to subtract a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_sub(env_t *env, expr_t *e)
{
  if(e == &NIL) return _int_expr(0);
  if(e->type != LIST_T) return _error("attempt to subtract a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_sub_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_sub_float(env, e->cdr, (float)e1->floatval); break;
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

static expr_t *eval_mul_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to multiply a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_mul_float(env, e->cdr, partial * (float)e1->intval); break;
  case FLOAT_T: result = eval_mul_float(env, e->cdr, partial * e1->floatval); break;
  default: result = _error("attempt to multiply a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_mul_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to multiply a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_mul_int(env, e->cdr, partial * e1->intval); break;
  case FLOAT_T: result = eval_mul_float(env, e->cdr, (float)partial * e1->floatval); break;
  default: result = _error("attempt to multiply a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_mul(env_t *env, expr_t *e)
{
  return eval_mul_int(env, e, 1);
}

static expr_t *eval_div_float(env_t *env, expr_t *e, float partial)
{
  if(e == &NIL) return _float_expr(partial);
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_div_float(env, e->cdr, partial / (float)e1->intval); break;
  case FLOAT_T: result = eval_div_float(env, e->cdr, partial / e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_div_int(env_t *env, expr_t *e, int partial)
{
  if(e == &NIL) return _int_expr(partial);
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_div_int(env, e->cdr, partial / e1->intval); break;
  case FLOAT_T: result = eval_div_float(env, e->cdr, (float)partial / e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_div(env_t *env, expr_t *e)
{
  if(e == &NIL) return _error("need at least two arguments for division");
  if(e->type != LIST_T) return _error("attempt to divide a non-numeric value"); // TODO this error message isn't quite sensible

  expr_t *e1 = _eval(env, e->car), *result;
  switch(e1->type) {
  case INTEGER_T: result = eval_div_int(env, e->cdr, e1->intval); break;
  case FLOAT_T: result = eval_div_float(env, e->cdr, (float)e1->floatval); break;
  default: result = _error("attempt to divide a non-numeric value");
  }
  
  // if we only had one operand, that's an error!
  if(e->cdr == &NIL) result = _error("need at least two arguments for division");

  // TODO how can we get rid of this in-eval cleanup?
  //we must clean up after ourselves
  _free_expr(e1);
  return result;
}

static expr_t *eval_var_def(env_t *env, expr_t *id_expr, expr_t *value)
{
  if(id_expr->type != IDENTIFIER_T) return _error("attempt to define a variable without a valid identifier");
  char *id = id_expr->strval;

  // fprintf(stderr, "putting: %s->%i\n", id, value->intval);
  put_var(env, id, value);

  return _id_expr(strdup(id));
}

static expr_t *eval_fun_def(env_t *env, expr_t *args)
{
  expr_t *e = args;
  if((e == &NIL) || (e->type != LIST_T)) return _error("too few arguments for defun");
  if(e->car->type != IDENTIFIER_T) return _error("attempt to define a function without a valid identifier");
  char *id = e->car->strval;
  e = e->cdr;

  // note: at this point if the function has been defined correctly
  //       then e->car holds our formal params and e->cdr->car holds
  //       our function body
  put_fun(env, id, e);
  return _id_expr(strdup(id));
}

static expr_t *eval_fun_call(env_t *env, char *id, expr_t *args)
{
  // grab the function from env
  expr_t *fundef = get_fun(env, id);

  // pull out our formal params and function body
  expr_t *formals = fundef->car;   // TODO ERROR CHECKING!
  expr_t *body = fundef->cdr->car; // TODO ERROR CHECKING!

  // new environment in which to evaluate the function
  env_t funenv;
  init_env(&funenv, env);

  // bind our formal params to our args
  expr_t *formal, *arg;
  for(formal = formals, arg = args; 
      ((formal != &NIL) && (arg != &NIL)); 
      formal = formal->cdr, arg = arg->cdr) {

    expr_t *e1 = _eval(env, arg->car);
    put_var(&funenv, formal->car->strval, e1); // TODO ERROR CHECKING!

    // we must clean up after ourselves
    _free_expr(e1);
  }

  // evaluate the function
  expr_t *result = _eval(&funenv, body);
  
  // clean up the function and the temporary environment
  _free_expr(fundef);
  free_env(&funenv);

  // and return our result!
  return result;
}

static expr_t *eval_let(env_t *env, expr_t *e) // expr_t *defs, expr_t *let)
{
  env_t letenv;
  init_env(&letenv, env);

  expr_t *defs = e->car; // TODO ERROR CHECKING!
  expr_t *body = e->cdr->car; // TODO ERROR CHECKING!

  expr_t *def;
  // evaluate the variable defs into the new env
  for(def = defs; def != &NIL; def = def->cdr) {
    expr_t *id_expr = def->car->car; // TODO error checking!
    expr_t *value = _eval(env, def->car->cdr->car); // TODO error checking!
    id_expr = eval_var_def(&letenv, id_expr, value); // TODO error checking!
    _free_expr(id_expr);
    _free_expr(value);
  }

  // evaluate the let in the new env
  expr_t *result = _eval(&letenv, body);

  // cleanup and return
  free_env(&letenv);
  return result;
}

static expr_t *eval_op(env_t *env, enum PUNKY_OP_TYPE op, expr_t *e)
{
  switch(op) {
    /* arithmetic operations */
  case ADD_OP: return eval_add(env, e);
  case SUB_OP: return eval_sub(env, e);
  case MUL_OP: return eval_mul(env, e);
  case DIV_OP: return eval_div(env, e);

    /* list operations */
  case CAR_OP: {
    expr_t *args = eval_args(env, e, 1), *result = _clone_expr(args->car->car);
    _free_expr(args);
    return result;
  }break; 
  case CDR_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car), *result = _clone_expr(e1->cdr); 
    _free_expr(e1);
    return result; 
  }break;
  case CONS_OP: { // TODO error checking!
    return _list_expr(_eval(env, e->car), _eval(env, e->cdr->car));
  }break;
  case LIST_OP: {
    expr_t *e1 = _eval(env, e->car);
    if(e1->type == ERROR_T) return e1;

    expr_t *ptr, *result = _list_expr(e1, &NIL), *r_ptr = result;
    for(ptr = e->cdr; ptr->type == LIST_T; ptr = ptr->cdr) {
      e1 = _eval(env, ptr->car);
      if(e1->type == ERROR_T) {
	_free_expr(result);
	return e1;
      }
      r_ptr->cdr = _list_expr(e1, &NIL);
      r_ptr = r_ptr->cdr;
    }
    return result;
  }break;

    /* variable/function definition */
  case DEFVAR_OP: { // TODO error checking!
    expr_t *value = _eval(env, e->cdr->car);
    expr_t *result = eval_var_def(env, e->car, value);
    _free_expr(value);
    return result;
  }break;
  case DEFUN_OP: return eval_fun_def(env, e);

    /* misc operations */
  case LET_OP: return eval_let(env, e);
  case QUOTE_OP: return _clone_expr(e->car); // TODO error checking!

    /* string operations */
  case SUBSTR_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car); // the string
    int pos = 0, len = strlen(e1->strval);
    if(e->cdr != &NIL) pos = _eval(env, e->cdr->car)->intval;
    if(e->cdr->cdr != &NIL) len = _eval(env, e->cdr->cdr->car)->intval;
    char *p = malloc(sizeof(char)*len);
    strncpy(p, e1->strval+pos, len);
    _free_expr(e1);
    return _str_expr(p);
  }break;
    
    /* boolean operations */
  case NOT_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car);
    e1->intval = !(e1->intval);
    return e1;
  }break;
  case AND_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car);
    expr_t *e2 = _eval(env, e->cdr->car);
    e1->intval = (e1->intval && e2->intval);
    _free_expr(e2);
    return e1;
  }break;
  case OR_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car);
    expr_t *e2 = _eval(env, e->cdr->car);
    e1->intval = (e1->intval || e2->intval);
    _free_expr(e2);
    return e1;
  }break;

    /* program control */
  case IF_OP: { // TODO error checking!
    expr_t *e1 = _eval(env, e->car), *result;
    if(e1->intval) {
      result = _eval(env, e->cdr->car);
    } else {
      result = (e->cdr->cdr == &NIL) ? &NIL :_eval(env, e->cdr->cdr->car);
    }
    _free_expr(e1);
    return result;
  }break;

    /* equality operations */
  case EQUAL_OP:
  case LT_OP:
  case GT_OP:
  case LE_OP:
  case GE_OP:
    
  default: return _error("unknown op type");
  }
}

static expr_t *eval_list(env_t *env, expr_t *car, expr_t *cdr)
{
  // the only options we should have here:
  //     IDENTIFIER_T, OP_T
  switch(car->type) {
  case IDENTIFIER_T: // function call
    return eval_fun_call(env, car->strval, cdr);

  case OP_T: return eval_op(env, car->op, cdr);

  default: return _error("i don't know how to evaluate this kind of list");
  }
}

static expr_t *_eval(env_t *env, expr_t *e)
{
  // if(config.debug) { fprintf(stderr, "eval: %s: ", type_to_string(e->type)); print(e); }
  expr_t *result;

  switch(e->type) {
    
  case LIST_T: {
    result = eval_list(env, e->car, e->cdr);
  }break;

    // i tried being "fancy" and re-using these types. in the end, it
    // causes a lot less headache by just returning a copy (double frees ftl)
  case BOOL_T: result = _bool_expr(e->intval); break;
  case INTEGER_T: result = _int_expr(e->intval); break;
  case FLOAT_T: result = _float_expr(e->floatval); break;
  case STRING_T: result = e; e->_ref++; break;

  case IDENTIFIER_T: {
    result = get_var(env, e->strval);
    if(!result) result = _error("undefined variable"); // TODO need to parameterize this
  }break;

  case NIL_T:
  case ERROR_T: result = e; break;
    
  default: result = _error("attempted evaluation of unknown expression type");
  }

  if(!(e->_ref)) _free_expr(e);
  return result;
}

punky_t *eval(punky_t *p)
{
  expr_t *e = _eval(&p->env, p->e);
  // _free_expr(p->e); // free up whatever we parsed
  p->e = e;
  return p;
}
