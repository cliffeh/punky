#include "env.h"

#include "alloc.h"
#include <stdlib.h>
#include <string.h>

void
env_init (environment *env, environment *parent)
{
  memset (env, 0, sizeof (*env));
  env->parent = parent;
}

sexpr *
env_get (environment *env, const char *key)
{
  for (entry *ent = env->entries; ent; ent = ent->next)
    {
      if (strcmp (key, ent->key) == 0) // found it
        return sexpr_copy (ent->value);
    }

  if (env->parent)
    return env_get (env->parent, key);

  // didn't find it
  return new_err ("unbound variable '%s'", key);
}

void
env_set (environment *env, const char *key, sexpr *value)
{
  for (entry *ent = env->entries; ent; ent = ent->next)
    {
      if (strcmp (key, ent->key) == 0) // found existing entry
        {
          sexpr_free (ent->value);
          ent->value = value;
          return;
        }
    }

  // didn't find it
  entry *ent = calloc (1, sizeof (entry));
  ent->key = strdup (key);
  ent->value = value;
  // insert at head
  ent->next = env->entries;
  env->entries = ent;
  env->count++;
}

sexpr *
env_del (environment *env, const char *key)
{
  entry *prev = env->entries;
  for (entry *ent = env->entries; ent; ent = ent->next)
    {
      if (strcmp (key, ent->key) == 0) // found it
        {
          prev->next = ent->next;
          free (ent->key);
          sexpr_free (ent->value);
          free (ent);
          if (--env->count == 0)
            env->entries = 0;
          return new_ident (key);
        }
      prev = ent;
    }
  // return the key even if we didn't find it
  return new_ident (key);
}

void
env_destroy (environment *env)
{
  entry *ent = env->entries;
  while (ent)
    {
      entry *next = ent->next;
      free (ent->key);
      sexpr_free (ent->value);
      free (ent);
      ent = next;
    }
}
