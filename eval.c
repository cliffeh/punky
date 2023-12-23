#include "alloc.h"
#include "builtin.h"
#include "env.h"
#include "punky.h"
#include <stdlib.h>

sexpr NIL = { .s_type = S_NIL };

static sexpr *
sexpr_apply_function (environment *env, const sexpr *params, const sexpr *body,
                      const sexpr *args)
{
}

static sexpr *
sexpr_apply (environment *env, const sexpr *e, const sexpr *args)
{
  switch (e->s_type)
    {
    case S_IDENT:
      {
        sexpr *fun = env_get (env, e->sval);
        if (fun->s_type != S_FUN)
          {
            sexpr *err = new_err ("cannot apply expression");
            err->cdr = fun;
            return err;
          }
        sexpr *result = sexpr_apply_function (env, fun->car, fun->cdr, args);
        sexpr_free (fun);
        return result;
      }
      break;

    case S_FUN: // anonymous function
      return sexpr_apply_function (env, e->car, e->cdr, args);

    case S_BUILTIN:
      return sexpr_apply_builtin (env, e, args);

    default: // TODO better error message
      return new_err ("cannot apply expression type: %d", e->s_type);
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
      return sexpr_apply (env, e->car, e->cdr);
    case S_BUILTIN:
      return new_err ("<builtin>: %s", e->sval);
    default:
      return new_err ("eval: unknown expression type %i", e->s_type);
    }
}
