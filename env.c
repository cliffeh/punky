#include "env.h"

#include <stdlib.h>
#include <string.h>

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

void *
env_get (environment *env, const char *key)
{
  entry *e = env_find_entry (env, key);
  if (e)
    return e->value;
  return 0;
}

void
env_set (environment *env, const char *key, void *value)
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
}
