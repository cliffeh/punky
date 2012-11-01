#include "punky.h"
#include "env.h"

static expr_t *_eval(env_t *env, expr_t *e);

static expr_t *eval_add_float(env_t *env, expr_t *operands, float partial)
{
  expr_t *result = _float_expr(partial), *e, *e1;
  for(e = operands; (e != &NIL); e = e->cdr) {
    //    if(config.debug) { fprintf(stderr, "eval_add_float: current value: %f\n", result->floatval); }
    e1 = _eval(env, e->car);
    switch(e1->type) {
    case INTEGER_T: result->floatval += (float)e1->intval; break;
    case FLOAT_T: result->floatval += e1->floatval; break;
    default: {
      // clean up our partial result
      _free_expr(result);
      if(e1 == &ERROR) return e1;
      _free_expr(e1);
      return _error("attempt to add a non-numeric value");
    }
    }
    // we must clean up after ourselves
    _free_expr(e1);
  }
  // if(config.debug) { fprintf(stderr, "eval_add_float: final value: %f\n", result->floatval); }
  return result;
}

static expr_t *eval_add_int(env_t *env, expr_t *operands, int partial)
{
  expr_t *result = _int_expr(partial), *e, *e1;
  for(e = operands; e != &NIL; e = e->cdr) {
    // if(config.debug) { fprintf(stderr, "eval_add_int: current value: %i\n", result->intval); }
    e1 = _eval(env, e->car);
    switch(e1->type) {
      // so long as we keep seeing INTEGERs, we'll use integer operations
    case INTEGER_T: result->intval += e1->intval; break;
      // once we see a FLOAT, it's all floating-point from there on out
    case FLOAT_T: {
      float partial = ((float)result->intval)+e1->floatval;
      _free_expr(result);
      _free_expr(e1);
      return eval_add_float(env, e->cdr, partial);
    }
    default: {
      // clean up our partial result
      _free_expr(result);
      if(e1 == &ERROR) return e1;
      _free_expr(e1);
      return _error("attempt to add a non-numeric value");
    }
    }
    // we must clean up after ourselves
    _free_expr(e1);
  }
  // if(config.debug) { fprintf(stderr, "eval_add_int: final value: %i\n", result->intval); }
  return result;
}

static expr_t *eval_add(env_t *env, expr_t *operands)
{
  return eval_add_int(env, operands, 0);
}

static expr_t *eval_sub(env_t *env, expr_t *operands)
{
  expr_t *result, *e = operands, *e1;

  // if we have no operands, we return 0
  if(e == &NIL) return _int_expr(0);

  // if we have only 1 operand, we'll return the negative of that value
  e1 = _eval(env, e->car);
  switch(e1->type) {
  case INTEGER_T: result = _int_expr(e1->intval); break;
  case FLOAT_T: result = _float_expr(e1->floatval); break;
  case ERROR_T: return e1; // we'll pass the error along
  default: _free_expr(e1); return _error("attempt to subtract a non-numeric value");
  }
  // we must clean up after ourselves
  _free_expr(e1);
  e = e->cdr;
  if(e == &NIL) {
    switch(result->type) {
    case INTEGER_T: result->intval = -result->intval; break;
    case FLOAT_T: result->floatval = -result->floatval; break;
    default: {} // this should never happen!
    }
    return result;
  }
  
  // if we have more than one operand, we'll subtract the summed result
  // of the remaining operands from the first
  e1 = eval_add(env, e);

  // this is the only way we can end up with an INTEGER result
  if((result->type == INTEGER_T) && (e1->type == INTEGER_T)) {
    result->intval -= e1->intval;
  } else if(e1->type == ERROR_T) { 
    free(result); 
    return e1; 
  } else { // otherwise, we've got a float
    result = _to_float(result);
    e1 = _to_float(e1);
    result->floatval -= e1->floatval;
  }

  // we must clean up after ourselves
  _free_expr(e1);

  return result;
}

static expr_t *eval_mul_float(env_t *env, expr_t *operands, float partial)
{
  expr_t *result = _float_expr(partial), *e, *e1;
  for(e = operands; e != &NIL; e = e->cdr) {
    e1 = _eval(env, e->car);
    switch(e1->type) {
    case INTEGER_T: result->floatval *= ((float)e1->intval); break;
    case FLOAT_T: result->floatval *= e1->floatval; break;
    default: {
      // clean up our partial result
      _free_expr(result);
      if(e1 == &ERROR) return e1;
      _free_expr(e1);
      return _error("attempt to multiply a non-numeric value");
    }
    }
    // we must clean up after ourselves
    _free_expr(e1);
  }
  return result;
}

static expr_t *eval_mul_int(env_t *env, expr_t *operands, int partial)
{
  expr_t *result = _int_expr(partial), *e, *e1;
  // if(config.debug) { fprintf(stderr, "eval_mul_int: current value: %i\n", result->intval); }
  for(e = operands; e != &NIL; e = e->cdr) {
    e1 = _eval(env, e->car);
    switch(e1->type) {

    case INTEGER_T: result->intval *= e1->intval; break;
    case FLOAT_T: {
      float partial = ((float)result->intval)*e1->floatval;
      _free_expr(result);
      _free_expr(e1);
      return eval_mul_float(env, e->cdr, partial);
    }
    default: {
      // clean up our partial result
      _free_expr(result);
      if(e1 == &ERROR) return e1;
      _free_expr(e1);
      return _error("attempt to multiply a non-numeric value");
    }
    }
    // we must clean up after ourselves
    _free_expr(e1);
  }
  return result;
}


static expr_t *eval_mul(env_t *env, expr_t *operands)
{
  return eval_mul_int(env, operands, 1);
}

static expr_t *eval_div(env_t *env, expr_t *operands)
{
  int result, is_float=0;
  float fresult;
  expr_t *e = operands, *e1, *ret;
  
  // first we must check that we have at least two operands
  if(e == &NIL || e->cdr == &NIL) // || e->cdr->car == &NIL)
    return _error("division attempted with less than 2 operands");

  // check the type of the first operand
  e1 = _eval(env, e->car);
  switch(e1->type){
  case INTEGER_T: result = e1->intval; break;
  case FLOAT_T: fresult = e1->floatval; is_float=1; break;
  case ERROR_T: return e1; // we'll pass the error along
  default: _free_expr(e1); return _error("attempt to divide with a non-numeric numerator");
  }

  // we must clean up after ourselves
  _free_expr(e1); 
  
  // multiply the rest of the operands to get the denominator
  e1 = is_float ? eval_mul_float(env, e->cdr, 1.0) : eval_mul_int(env, e->cdr, 1);

  // divide the numerator with the denominator, of course
  // checking to make sure the denominator is non-zero
  switch(e1->type) {
  case INTEGER_T: {
    if(e1->intval == 0) {
     ret = _error("attempt to divide by zero");
    } else {
      ret = is_float ? _float_expr(fresult/((float)e1->intval)) : _int_expr(result/e1->intval);
    }
  }break;
  case FLOAT_T: {
    if(e1->floatval == 0.0) {
      ret = _error("attempt to divide by zero");
    } else {
      ret = is_float ? _float_expr(fresult/e1->floatval) : _float_expr(((float)result)/e1->floatval);
    }
  }break;
  case ERROR_T: return e1; // we'll pass the error along
  default: ret = _error("unknown denominator type");
  }

  // we must clean up after ourselves
  _free_expr(e1);
  
  return ret;
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

static expr_t *eval_list(env_t *env, expr_t *car, expr_t *cdr)
{
  // the only options we should have here:
  //     IDENTIFIER_T, OP_T
  switch(car->type) {
  case IDENTIFIER_T: // function call
    return eval_fun_call(env, car->strval, cdr);

  case OP_T: {
    switch(car->op) {
    case ADD_OP: return eval_add(env, cdr);
    case SUB_OP: return eval_sub(env, cdr);
    case MUL_OP: return eval_mul(env, cdr);
    case DIV_OP: return eval_div(env, cdr);
    case CAR_OP: return _eval(env, cdr->car)->car; // TODO error checking!
    case CDR_OP: return _eval(env, cdr->car)->cdr; // TODO error checking!
    case QUOTE_OP: return _clone_expr(cdr->car); // TODO error checking!
    case DEFVAR_OP: { // TODO error checking!
      expr_t *value = _eval(env, cdr->cdr->car);
      expr_t *result = eval_var_def(env, cdr->car, value);
      _free_expr(value);
      return result;
    }break;
    case DEFUN_OP: return eval_fun_def(env, cdr);
    case LET_OP: return eval_let(env, cdr);

    default: return _error("unknown op type");
    }
  }break;
  }
}

static expr_t *_eval(env_t *env, expr_t *e)
{
  // if(config.debug) { fprintf(stderr, "eval: %s: ", type_to_string(e->type)); print(e); }
  expr_t *result;

  if(!e) { return _error("OH SHIZZLE! WE HAVE A NULL SHIT!\n"); }

  switch(e->type) {
    
  case LIST_T: {
    result = eval_list(env, e->car, e->cdr);
  }break;

    // i tried being "fancy" and re-using these types. in the end, it
    // causes a lot less headache by just returning a copy (double frees ftl)
  case INTEGER_T: result = _int_expr(e->intval); break; // result = _int_expr(e->intval); break;
  case FLOAT_T: result = _float_expr(e->floatval); break;
  case STRING_T: result = _str_expr(strdup(e->strval)); break;

  case IDENTIFIER_T: {
    result = get_var(env, e->strval);
    if(!result) result = _error("undefined variable"); // TODO need to parameterize this
  }break;

  case NIL_T:
  case ERROR_T: result = e; break;
    
  default: result = _error("attempted evaluation of unknown expression type");
  }

  // if(e != result) _free_expr(e);
  return result;
}

punky_t *eval(punky_t *p)
{
  expr_t *e = _eval(&p->env, p->e);
  _free_expr(p->e); // free up whatever we parsed
  p->e = e;
  return p;
}
