#include "env.h"
#include "alloc.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

void
env_init (environment *env, environment *parent)
{
  memset (env, 0, sizeof (*env));
  env->entries = (sexpr *)&NIL;
  env->parent = parent;
}

sexpr *
env_get (environment *env, const char *key)
{
  for (sexpr *ptr = env->entries; ptr != &NIL; ptr = ptr->cdr)
    {
      sexpr *entry = ptr->car;
      if (strcmp (key, entry->car->sval) == 0) // found it
        {
          if (!entry->cdr)
            return new_err (0, "unbound variable '%s'", key);
          return sexpr_copy (entry->cdr);
        }
    }

  if (env->parent)
    return env_get (env->parent, key);

  // didn't find it
  return new_err (0, "unbound variable '%s'", key);
}

void
env_set (environment *env, const char *key, sexpr *value)
{
  sexpr *ptr, *entry;
  for (ptr = env->entries; ptr != &NIL; ptr = ptr->cdr)
    {
      entry = ptr->car;
      if (strcmp (key, entry->car->sval) == 0) // found existing entry
        {
          sexpr_free (entry->cdr);
          entry->cdr = value;
          return;
        }
    }

  // didn't find it
  entry = new_list (new_ident (key), value);

  // insert at head
  ptr = new_list (entry, env->entries);
  env->entries = ptr;
}

void
env_destroy (environment *env)
{
  sexpr_free (env->entries);
}
