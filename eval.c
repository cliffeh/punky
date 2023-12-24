#include "eval.h"
#include "constants.h"
#include "alloc.h"

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
          return new_err (0, "non-identifier in parameter list");
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
    return new_err (0, "not enough arguments to function");
  if (args != &NIL)
    return new_err (0, "too many arguments to function");

  return (sexpr *)&NIL;
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
      return new_err (result, "error evaluating function");
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
    default:
      sexpr *fun = sexpr_eval (env, e);
      if (fun->s_type != S_FUN)
        return new_err (fun, "attempt to apply inapplicable type");

      sexpr *result = sexpr_apply_function (env, fun->car, fun->cdr, args);
      sexpr_free (fun);
      return result;
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
      return (sexpr *)&NIL;
    case S_BOOL:
      return (sexpr *)e;
    case S_INT:
      return new_int (e->ival);
    case S_STR:
      return new_str (e->sval);
    case S_QUOTE:
      return sexpr_copy (e->car);
    case S_IDENT:
      return env_get (env, e->sval);
    case S_FUN:
      return new_err (0, "<function>");
    case S_LIST:
      return sexpr_apply (env, e->car, e->cdr);
    case S_BUILTIN:
      return new_err (0, "<builtin> %s", e->sval);
    default:
      return new_err (0, "eval: unknown expression type %i", e->s_type);
    }
}
