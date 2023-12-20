#include "punky.h"
#include "alloc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sexpr *
sexpr_eval (environment *env, sexpr *e)
{
  switch (e->s_type)
    {
    case S_ERR:
    case S_NIL:
    case S_INT:
    case S_STR:
    case S_BUILTIN:
      return e;
    case S_QUOTE:
      return e->car;
    case S_IDENT:
      return env_get (env, e->sval);
    case S_PAIR:
      return new_err ("eval: cannot evaluate pair");
    // case S_LIST
      // sexpr *fun = eval(e->car);
      // sexpr *args = 
    default:
      return new_err ("eval: unknown expression type %i", e->s_type);
    }
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
