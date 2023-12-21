#include "alloc.h"
#include "punky.h"
#include <stdlib.h>

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

static sexpr *
sexpr_apply_builtin (environment *env, sexpr *builtin, const sexpr *args)
{
  switch (builtin->b_type)
    {
    case B_TYPE_DEFINE:
      return builtin_apply_define (env, args);
    case B_TYPE_PLUS:
      return builtin_apply_plus (env, args);
    case B_TYPE_MINUS:
      return builtin_apply_minus (env, args);
    default:
      return new_err ("unknown/unimplemented builtin %d\n", builtin->b_type);
    }
}

sexpr *
sexpr_apply (environment *env, sexpr *fun, const sexpr *args)
{
  if (fun->s_type == S_BUILTIN)
    return sexpr_apply_builtin (env, fun, args);
  return new_err ("I don't know how to apply functions yet!");
}

sexpr *
sexpr_eval (environment *env, const sexpr *e)
{
  switch (e->s_type)
    {
    case S_ERR:
      return sexpr_copy (e);
    case S_NIL:
      return &NIL;
    case S_INT:
      return new_int (e->ival);
    case S_STR:
      return new_str (e->sval);
    case S_QUOTE:
      return sexpr_copy (e->car);
    case S_IDENT:
      return env_get (env, e->sval);
    case S_PAIR:
      return new_err ("eval: cannot evaluate pair");
    case S_LIST:
      return sexpr_apply (env, e->car, e->cdr);
    case S_BUILTIN:
      return new_err ("<builtin>: %s", e->sval);
    default:
      return new_err ("eval: unknown expression type %i", e->s_type);
    }
}
