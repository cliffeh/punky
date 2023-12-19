#include "punky.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sexpr *
eval (environment *env, sexpr *e)
{
  switch (e->type)
    {
    case SEXPR_ERR:
    case SEXPR_NIL:
    case SEXPR_INT:
    case SEXPR_STR:
      return e;
    case SEXPR_QUOTE:
      return e->car;
    case SEXPR_IDENT:
      return env_get (env, e->sval);
    default:
      return new_err ("eval: unknown expression type");
    }
}

void
print (FILE *out, int flags, sexpr *e)
{
  int depth = flags & 0x0000FFFF;
  switch (e->type)
    {
    case SEXPR_ERR:
      fprintf (out, "error: %s", e->sval);
      break;
    case SEXPR_NIL:
      fprintf (out, "nil"); // ()
      break;
    case SEXPR_INT:
      fprintf (out, "%d", e->ival);
      break;
    case SEXPR_STR:
      fprintf (out, "\"%s\"", e->sval);
      break;
    case SEXPR_QUOTE:
      fprintf (out, "(quote ");
      print (out, flags + 1, e->car);
      fprintf (out, ")");
      break;
    case SEXPR_IDENT:
      fprintf (out, "%s", e->sval);
      break;
    case SEXPR_LIST:
      fprintf (out, "(");
      break;
      // for(sexpr car )
    default:
      fprintf (stderr, "print: unknown expression type\n");
    }

  if (depth == 0)
    fprintf (out, "\n");
}

/* environment */
static entry *
env_find_entry (environment *env, const char *key)
{
  for (entry *entry = &env->handle; entry->next; entry = entry->next)
    {
      if (strcmp (key, entry->next->key) == 0)
        return entry->next;
    }
  // didn't find it
  return 0;
}

sexpr *
env_get (environment *env, const char *key)
{
  entry *e = env_find_entry (env, key);
  if (e)
    return e->value;
  return new_err ("eval: unbound variable '%s'", key);
}

sexpr *
env_put (environment *env, const char *key, sexpr *value)
{
  entry *e = env_find_entry (env, key);
  if (e)
    { // replace
      free (e->value);
      e->value = value;
    }
  else
    { // create
      e = calloc (1, sizeof (entry));
      e->key = strdup (key);
      e->value = value;
      // insert at head
      e->next = env->handle.next;
      env->handle.next = e;
    }
  return value;
}

/* memory management */
sexpr *
new_err (const char *fmt, ...)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_ERR;

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
new_nil ()
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_NIL;
  return e;
}

sexpr *
new_int (int ival)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_INT;
  e->ival = ival;
  return e;
}

sexpr *
new_str (const char *str)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_STR;
  e->sval = strdup (str);
  return e;
}

sexpr *
new_quote (sexpr *q)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_QUOTE;
  e->car = q;
  return e;
}

sexpr *
new_ident (const char *name)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_IDENT;
  e->sval = strdup (name);
  return e;
}

sexpr *
new_pair (sexpr *car, sexpr *cdr)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_PAIR;
  e->car = car;
  e->cdr = cdr;
  return e;
}

sexpr *
new_list (sexpr *car, sexpr *cdr)
{
  sexpr *e = calloc (1, sizeof (sexpr));
  e->type = SEXPR_LIST;
  e->car = car;
  e->cdr = cdr;
  return e;
}
