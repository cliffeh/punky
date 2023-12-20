#include "punky.h"
#include "alloc.h"

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
      return e->car;
    case S_IDENT:
      sexpr *r = env_get (env, e->sval);
      return r ? r : new_err ("unbound variable '%s'", e->sval);
    case S_PAIR:
      return new_err ("eval: cannot evaluate pair");
    // case S_LIST
    // sexpr *fun = eval(e->car);
    // sexpr *args =
    case S_BUILTIN:
      return new_err ("<builtin>: %s", e->sval);
    default:
      return new_err ("eval: unknown expression type %i", e->s_type);
    }
}
