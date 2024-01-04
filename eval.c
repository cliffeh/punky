#include "eval.h"
#include "alloc.h"
#include "constants.h"

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
      else if ((value = args->car->eval (args->car, penv))->s_type == S_ERR)
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

sexpr *
sexpr_eval_builtin (const sexpr *self, environment *env)
{
  // return new_err (0, "<builtin> %s", self->sval);
  return (sexpr *)self; // TODO is this right?
}

sexpr *
sexpr_eval_copy (const sexpr *self, environment *env)
{
  return sexpr_copy (self);
}

sexpr *
sexpr_eval_ident (const sexpr *self, environment *env)
{
  return env_get (env, self->sval);
}

sexpr *
sexpr_eval_function (const sexpr *self, environment *env)
{
  return new_err (0, "<function>");
}

sexpr *
sexpr_eval_list (const sexpr *self, environment *env)
{
  sexpr *fun = self->car->eval (self->car, env);
  sexpr *result = fun->apply (fun, self->cdr, env);
  sexpr_free (fun);
  return result;
}

sexpr *
sexpr_eval_quote (const sexpr *self, environment *env)
{
  return sexpr_copy (self->car);
}

sexpr *
sexpr_apply_function (const sexpr *self, const sexpr *args, environment *env)
{
  environment fenv;
  env_init (&fenv, env);

  sexpr *result = bind_params (&fenv, self->car, args, env);
  if (result != &NIL)
    {
      env_destroy (&fenv);
      return new_err (result, "error evaluating function");
    }

  result = self->cdr->eval (self->cdr, &fenv);
  env_destroy (&fenv);
  return result;
}

sexpr *
sexpr_apply_inapplicable (const sexpr *self, const sexpr *args,
                          environment *env)
{
  return new_err (0, "attempt to apply inapplicable type");
}
