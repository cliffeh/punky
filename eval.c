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
sexpr_apply_builtin (environment *env, sexpr *builtin, sexpr *args)
{
  switch (builtin->b_type)
    {
    case B_TYPE_DEFINE:
      return builtin_apply_define (env, args);
    case B_TYPE_PLUS:
      return builtin_apply_plus (env, args);
    default:
      return new_err ("unknown/unimplemented builtin %d\n", builtin->b_type);
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
  sexpr *r = 0;

  switch (e->s_type)
    {
    case S_ERR:
    case S_NIL:
    case S_INT:
    case S_STR:
      r = e;
      break;
    case S_QUOTE:
      r = e->car;
      free (e);
      break;
    case S_IDENT:
      r = env_get (env, e->sval);
      sexpr_free (e);
      break;
    case S_PAIR:
      r = new_err ("eval: cannot evaluate pair");
      sexpr_free (e);
      break;
    case S_LIST:
      r = sexpr_apply (env, e->car, e->cdr);
      sexpr_free (e);
      break;
    case S_BUILTIN:
      r = new_err ("<builtin>: %s", e->sval);
      break;
    default:
      r = new_err ("eval: unknown expression type %i", e->s_type);
      sexpr_free (e);
      return r;
    }

  return r;
}
