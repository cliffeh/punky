#include "alloc.h"
#include "env.h"
#include "punky.h"

sexpr NIL = { .s_type = 0 };
sexpr B_DEFINE
    = { .s_type = S_BUILTIN, .b_type = B_TYPE_DEFINE, .sval = "define" };
sexpr B_MINUS = { .s_type = S_BUILTIN, .b_type = '-', .sval = "-" };
sexpr B_MULT = { .s_type = S_BUILTIN, .b_type = '*', .sval = "*" };
sexpr B_PLUS = { .s_type = S_BUILTIN, .b_type = '+', .sval = "+" };

static sexpr *
builtin_apply_define (environment *env, const sexpr *args)
{
  const sexpr *list = args, *key;
  if (list->s_type != S_LIST || (key = list->car)->s_type != S_IDENT)
    {
      return new_err ("malformed arguments to define");
    }

  if (list->cdr == &NIL)
    {
      return env_del (env, key->sval);
    }
  else if (list->cdr->s_type != S_LIST)
    {
      return new_err ("malformed arguments to define");
    }

  sexpr *value = sexpr_eval (env, list->cdr->car);

  if (value->s_type == S_ERR)
    return value;

  return env_set (env, key->sval, value);
}

static sexpr *
builtin_apply_plus (environment *env, const sexpr *args)
{
  int r = 0;
  const sexpr *list;
  for (list = args; list->s_type == S_LIST; list = list->cdr)
    {
      sexpr *e = sexpr_eval (env, list->car);
      if (e->s_type != S_INT)
        {
          if (e->s_type == S_ERR)
            return e;

          sexpr *err
              = new_err ("cannot perform arithmetic on non-integer type");
          err->cdr = e;
          return err;
        }
      r += e->ival;
      sexpr_free (e);
    }

  if (list != &NIL)
    return new_err ("malformed arguments to arithmetic");

  return new_int (r);
}

static sexpr *
builtin_apply_mult (environment *env, const sexpr *args)
{
  int r = 1;
  const sexpr *list;
  for (list = args; list->s_type == S_LIST; list = list->cdr)
    {
      sexpr *e = sexpr_eval (env, list->car);
      if (e->s_type != S_INT)
        {
          if (e->s_type == S_ERR)
            return e;

          sexpr *err
              = new_err ("cannot perform arithmetic on non-integer type");
          err->cdr = e;
          return err;
        }
      r *= e->ival;
      sexpr_free (e);
    }

  if (list != &NIL)
    return new_err ("malformed arguments to arithmetic");

  return new_int (r);
}

static sexpr *
builtin_apply_minus (environment *env, const sexpr *args)
{
  const sexpr *list = args;

  if (list->s_type != S_LIST)
    return new_err ("subtraction requires at least one argument");

  sexpr *e = sexpr_eval (env, list->car);
  if (e->s_type != S_INT)
    {
      if (e->s_type == S_ERR)
        return e;

      sexpr *err = new_err ("cannot perform arithmetic on non-integer type");
      err->cdr = e;
      return err;
    }

  int r = e->ival;
  if (args->cdr == &NIL)
    { // special case
      e->ival = -e->ival;
      return e;
    }
  sexpr_free (e);

  // guaranteed S_INT or S_ERR
  sexpr *rest = builtin_apply_plus (env, args->cdr);
  if (rest->s_type == S_ERR)
    return rest;

  r -= rest->ival;
  sexpr_free (rest);

  return new_int (r);
}

sexpr *
sexpr_apply_builtin (environment *env, sexpr *builtin, const sexpr *args)
{
  switch (builtin->b_type)
    {
    case B_TYPE_DEFINE:
      return builtin_apply_define (env, args);
    case B_TYPE_PLUS:
      return builtin_apply_plus (env, args);
    case B_TYPE_MULT:
      return builtin_apply_mult (env, args);
    case B_TYPE_MINUS:
      return builtin_apply_minus (env, args);
    default:
      return new_err ("unknown/unimplemented builtin %d\n", builtin->b_type);
    }
}
