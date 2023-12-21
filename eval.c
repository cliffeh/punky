#include "alloc.h"
#include "env.h"
#include "punky.h"
#include <stdlib.h>

sexpr *sexpr_apply_builtin (environment *env, sexpr *builtin,
                            const sexpr *args); // builtin.c

static sexpr *
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
