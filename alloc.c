#include "punky.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

sexpr NIL = { .s_type = 0 };

sexpr *
new_err (const char *fmt, ...)
{
  sexpr *e = calloc (1, sizeof (sexpr));
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
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_INT;
  e->ival = ival;
  return e;
}

sexpr *
new_str (const char *str)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_STR;
  e->sval = strdup (str);
  return e;
}

sexpr *
new_quote (sexpr *q)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_QUOTE;
  e->car = q;
  return e;
}

sexpr *
new_ident (const char *name)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_IDENT;
  e->sval = strdup (name);
  return e;
}

sexpr *
new_pair (sexpr *car, sexpr *cdr)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_PAIR;
  e->car = car;
  e->cdr = cdr;
  return e;
}

sexpr *
new_list (sexpr *car, sexpr *cdr)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_LIST;
  e->car = car;
  e->cdr = cdr;
  return e;
}

sexpr *
new_builtin (builtin_type b_type, const char *desc)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->s_type = S_BUILTIN;
  e->b_type = b_type;
  e->sval = strdup (desc);
  return e;
}
