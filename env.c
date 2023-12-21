#include "env.h"

#include "alloc.h" // for sexpr_free()
#include <stdlib.h>
#include <string.h>

void
env_init (environment *env)
{
  memset (env, 0, sizeof (*env));
}

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
  return new_err ("unbound variable '%s'", key);
}

void
env_set (environment *env, const char *key, sexpr *value)
{
  entry *e = env_find_entry (env, key);
  if (e)
    { // replace
      sexpr_free (e->value);
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
}

void env_destroy(environment *env)
{
    // TODO!
}