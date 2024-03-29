#include "alloc.h"
#include "constants.h"
#include "eval.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEXPR_ALLOC(e) calloc (1, sizeof (*e))

sexpr *
new_err (sexpr *stack, const char *fmt, ...)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_ERR;

  va_list args;

  va_start (args, fmt);
  int len = vsnprintf (0, 0, fmt, args) + 1;
  va_end (args);

  char *msg = calloc (len, sizeof (char));

  va_start (args, fmt);
  vsnprintf (msg, len, fmt, args);
  va_end (args);

  e->sval = msg;
  e->cdr = stack ? stack : (sexpr *)&NIL;

  e->eval = sexpr_eval_copy;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
new_int (int ival)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_INT;
  e->ival = ival;

  e->eval = sexpr_eval_copy;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
new_str (const char *str)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_STR;
  e->sval = strdup (str);

  e->eval = sexpr_eval_copy;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
new_quote (sexpr *q)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_QUOTE;
  e->car = q;

  e->eval = sexpr_eval_quote;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
new_ident (const char *name)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_IDENT;
  e->sval = strdup (name);

  e->eval = sexpr_eval_ident;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
new_fun (sexpr *params, sexpr *body)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_FUN;
  e->car = params;
  e->cdr = body;

  e->eval = sexpr_eval_function;
  e->apply = sexpr_apply_function;

  return e;
}

sexpr *
new_list (sexpr *car, sexpr *cdr)
{
  sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_LIST;
  e->car = car;
  e->cdr = cdr;

  e->eval = sexpr_eval_list;
  e->apply = sexpr_apply_inapplicable;

  return e;
}

sexpr *
sexpr_copy (const sexpr *e)
{
  switch (e->s_type)
    {
    case S_NIL:
    case S_BOOL:
    case S_BUILTIN:
      return (sexpr *)e; // this should be okay
    case S_ERR:
      return new_err (sexpr_copy (e->cdr), e->sval); // TODO is this necessary?
    case S_INT:
      return new_int (e->ival);
    case S_STR:
      return new_str (e->sval);
    case S_QUOTE:
      return new_quote (sexpr_copy (e->car));
    case S_IDENT:
      return new_ident (e->sval);
    case S_FUN:
      return new_fun (sexpr_copy (e->car), sexpr_copy (e->cdr));
    case S_LIST:
      return new_list (sexpr_copy (e->car), sexpr_copy (e->cdr));
    default:
      return new_err (0, "I don't know how to copy type %d", e->s_type);
    }
}

void
sexpr_free (sexpr *e)
{
  if (!e)
    return;

  switch (e->s_type)
    {
    case S_NIL:
    case S_BOOL:
    case S_BUILTIN:
      return;
    case S_ERR:
      free (e->sval);
      sexpr_free (e->cdr);
      break;
    case S_INT:
      break;
    case S_QUOTE:
      sexpr_free (e->car);
      break;
    case S_STR:
    case S_IDENT:
      free (e->sval);
      break;
    case S_FUN:
    case S_LIST:
      sexpr_free (e->car);
      sexpr_free (e->cdr);
      break;
    }
  free (e);
}
