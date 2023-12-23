#include "alloc.h"
#include "builtin.h"
#include "env.h"
#include "punky.h"
#include <stdlib.h>

sexpr NIL = { .s_type = S_NIL };

static sexpr *
bind_params (environment *fenv, const sexpr *params, const sexpr *args,
             environment *penv)
{
  sexpr *key, *value;
  for (; params->s_type == S_LIST && args->s_type == S_LIST;
       params = params->cdr, args = args->cdr)
    {
      if ((key = params->car)->s_type != S_IDENT)
        {
          return new_err ("non-identifier in parameter list");
        }
      // evaluate arguments in the parent env
      else if ((value = sexpr_eval (penv, args->car))->s_type == S_ERR)
        {
          return value;
        }
      else
        { // ...and set them in the function env
          env_set (fenv, key->sval, value);
        }
    }

  if (params != &NIL)
    {
      fprintf (stderr, "\n\n\nleftover params:\n\n\n");
      sexpr_print (stderr, 0, params);
      return new_err ("not enough arguments to function");
    }
  if (args != &NIL)
    return new_err ("too many arguments to function");

  return &NIL;
}

static sexpr *
sexpr_apply_function (environment *env, const sexpr *params, const sexpr *body,
                      const sexpr *args)
{
  environment fenv;
  env_init (&fenv, env);

  sexpr *result = bind_params (&fenv, params, args, env);
  if (result != &NIL)
    {
      env_destroy (&fenv);
      sexpr *err = new_err ("error evaluating function");
      err->cdr = result;
      return err;
    }

  result = sexpr_eval (&fenv, body);
  env_destroy (&fenv);
  return result;
}

static sexpr *
sexpr_apply (environment *env, const sexpr *e, const sexpr *args)
{
  switch (e->s_type)
    {
    case S_BUILTIN:
      return sexpr_apply_builtin (env, e, args);
    case S_FUN:
      return sexpr_apply_function (env, e->car, e->cdr, args);
    default:
      return new_err ("attempt to apply inapplicable type");
    }
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
    case S_FUN:
      return new_err ("<function>");
    case S_PAIR:
      return new_err ("eval: cannot evaluate pair");
    case S_LIST:
      sexpr *applicable = sexpr_eval (env, e->car);
      sexpr *result = sexpr_apply (env, applicable, e->cdr);
      sexpr_free (applicable);
      return result;
    case S_BUILTIN:
      return (sexpr *)e; // this should be fine
    default:
      return new_err ("eval: unknown expression type %i", e->s_type);
    }
}
