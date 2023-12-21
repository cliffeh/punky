#include "alloc.h"
#include "punky.h"
#include <stdlib.h>

static sexpr *
builtin_apply_define (environment *env, sexpr *args)
{
  return new_err ("I don't know how to do defines yet");
}

static sexpr *
builtin_apply_plus (environment *env, sexpr *args)
{
  int r = 0;
  sexpr *list;
  for (list = args; list->s_type == S_LIST; list = list->cdr)
    {
      sexpr *e = sexpr_eval (env, list->car);
      if (e->s_type != S_INT)
        {
          return new_err ("cannot perform addition on non-integer type");
        }
      r += e->ival;
    }

  if (list != &NIL)
    return new_err ("malformed input arguments to addition");

  return new_int (r);
}

static sexpr *
sexpr_apply_builtin (environment *env, sexpr *fun, sexpr *args)
{
  switch (fun->b_type)
    {
    case '+':
      return builtin_apply_plus (env, args);
    case B_DEFINE:
      return builtin_apply_define (env, args);
    default:
      return new_err ("unknown/unimplemented builtin %d\n", fun->b_type);
    }
}

sexpr *
sexpr_apply (environment *env, sexpr *fun, sexpr *args)
{
  if (fun->s_type == S_BUILTIN)
    return sexpr_apply_builtin (env, fun, args);
  return new_err ("I don't know how to apply functions yet!");
}

sexpr *
sexpr_eval (environment *env, sexpr *e)
{
  switch (e->s_type)
    {
    case S_ERR:
    case S_NIL:
    case S_INT:
    case S_STR:
      return e;
    case S_QUOTE:
      sexpr *q = e->car;
      free (e);
      return q;
    case S_IDENT:
      sexpr *r = env_get (env, e->sval);
      return r ? r : new_err ("unbound variable '%s'", e->sval);
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
