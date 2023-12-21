#include "punky.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

sexpr NIL = { .s_type = 0 };

#define SEXPR_ALLOC(e) calloc (1, sizeof (*e))

sexpr *
new_err (const char *fmt, ...)
{
  struct sexpr *e = SEXPR_ALLOC (e);
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

  return e;
}

sexpr *
new_int (int ival)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_INT;
  e->ival = ival;
  return e;
}

sexpr *
new_str (const char *str)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_STR;
  e->sval = strdup (str);
  return e;
}

sexpr *
new_quote (sexpr *q)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_QUOTE;
  e->car = q;
  return e;
}

sexpr *
new_ident (const char *name)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_IDENT;
  e->sval = strdup (name);
  return e;
}

sexpr *
new_pair (sexpr *car, sexpr *cdr)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_PAIR;
  e->car = car;
  e->cdr = cdr;
  return e;
}

sexpr *
new_list (sexpr *car, sexpr *cdr)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_LIST;
  e->car = car;
  e->cdr = cdr;
  return e;
}

sexpr *
new_builtin (builtin_type b_type, const char *desc)
{
  struct sexpr *e = SEXPR_ALLOC (e);
  e->s_type = S_BUILTIN;
  e->b_type = b_type;
  e->sval = strdup (desc);
  return e;
}

void
sexpr_free (sexpr *e)
{
  switch (e->s_type)
    {
    case S_NIL:
      return;
    case S_INT:
      break;
    case S_ERR: // TODO handle freeing the error stack better
      free (e->sval);
      break;
    case S_QUOTE:
      sexpr_free(e->car);
      break;
    case S_STR:
    case S_IDENT:
    case S_BUILTIN:
      free (e->sval);
      break;
    case S_PAIR:
    case S_LIST:
      sexpr_free (e->car);
      sexpr_free (e->cdr);
      break;
    }
  free (e);
}