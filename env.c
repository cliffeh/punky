#include "env.h"

#include "alloc.h"
#include <stdlib.h>
#include <string.h>

void
env_init (environment *env)
{
  memset (env, 0, sizeof (*env));
}

sexpr *
env_get (environment *env, const char *key)
{
  for (entry *ent = &env->handle; ent->next; ent = ent->next)
    {
      if (strcmp (key, ent->next->key) == 0)
        return sexpr_copy (ent->next->value);
    }

  if (env->parent)
    return env_get (env->parent, key);

  // didn't find it
  return new_err ("unbound variable '%s'", key);
}

sexpr *
env_set (environment *env, const char *key, sexpr *value)
{
  for (entry *ent = &env->handle; ent->next; ent = ent->next)
    {
      if (strcmp (key, ent->next->key) == 0)
        {
          sexpr_free (ent->next->value);
          ent->next->value = value;
          return new_ident (key);
        }
    }

  // didn't find it
  entry *ent = calloc (1, sizeof (entry));
  ent->key = strdup (key);
  ent->value = value;
  // insert at head
  ent->next = env->handle.next;
  env->handle.next = ent;

  return new_ident (key);
}

sexpr *
env_del (environment *env, const char *key)
{
  for (entry *ent = &env->handle; ent->next; ent = ent->next)
    {
      if (strcmp (key, ent->next->key) == 0)
        {
          entry *tmp = ent->next;
          ent->next = tmp->next;
          free (tmp->key);
          sexpr_free (tmp->value);
          free (tmp);
          return new_ident (key);
        }
    }
  // return the key even if we didn't find it
  return new_ident (key);
}

void
env_destroy (environment *env)
{
  entry *ent = &env->handle;
  while (ent->next)
    {
      entry *tmp = ent->next;
      ent->next = tmp->next;
      free (tmp->key);
      sexpr_free (tmp->value);
      free (tmp);
    }
}