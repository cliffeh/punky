#include "punky.h"

enum PUT_TYPE {
  VAR, FUN
};

static int hash(char *id)
{
  int value = 0, i = 0;
  char *p = id;

  // TODO get better hash.
  // we'll hash on the first 3 characters of the id
  for(i = 0; *(p+i) && (i<3); i++) {
    value += *(p+i);
  }
  return (value % ENV_BUCKETS);
}

void init_env(env_t *env, env_t *parent)
{
  int i;
  for(i = 0; i < ENV_BUCKETS; i++) {
    env->funs[i] = 0;
    env->vars[i] = 0;
  }
  env->parent = parent;
}

static entry_t *_find_entry(env_t *env, char *id, entry_t *bucket)
{
  entry_t *entry;
  for(entry = bucket; entry; entry = entry->next) {
    if(strcmp(id, entry->id) == 0) {
      return entry;
    }
  }
  // sorry, didn't find it
  return 0;
}

void put(env_t *env, char *id, expr_t *e, enum PUT_TYPE type)
{
  // we need to know which bucket we're in
  int i = hash(id);

  // and which set of entries to look at
  entry_t **entries;
  switch(type) {
  case VAR: entries = env->vars; break;
  case FUN: entries = env->funs; break;
  default: {} // shouldn't happen, but what do we do in this case?
  }

  // see if an entry with this id already exists
  entry_t *entry = _find_entry(env, id, entries[i]);

  if(entry) {
    // we found it! we'll release the old expr_t and replace it with the new one
    _free_expr(entry->e);
  } else {  
    // we didn't find it, so let's create a new entry
    entry = malloc(sizeof(entry_t));
    entry->id = strdup(id);

    // make sure we keep track of any already-existing entries in this bucket
    entry->next = entries[i];
    
    // we'll append our new entry at the head of the list
    entries[i] = entry;
  }

  // clone the expression and stick it in our table
  entry->e = _clone_expr(e);
}

expr_t *get(env_t *env, char *id, enum PUT_TYPE type) {
  // we need to know what bucket we're in
  int i = hash(id);

  // and which set of entries to look at
  entry_t **entries;
  switch(type) {
  case VAR: entries = env->vars; break;
  case FUN: entries = env->funs; break;
  default: {} // shouldn't happen, but what do we do in this case?
  }

  entry_t *entry = _find_entry(env, id, entries[i]);
  if(entry) { 
    // we found it! let's return it...
    return _clone_expr(entry->e); // protect our entries by returning a clone
  } else if(env->parent) {
    return get(env->parent, id, type);
  }

  // sorry, that var isn't defined here
  return 0;
}

expr_t *get_var(env_t *env, char *id)
{
  return get(env, id, VAR);
}

expr_t *get_fun(env_t *env, char *id)
{
  return get(env, id, FUN);
}

void *put_var(env_t *env, char *id, expr_t *vardef)
{
  put(env, id, vardef, VAR);
}

void *put_fun(env_t *env, char *id, expr_t *fundef)
{
  put(env, id, fundef, FUN);
}

void free_entry(entry_t *entry) 
{
  entry_t *e = entry, *next;
  while(e) {
    free(e->id);
    _free_expr(e->e);
    next = e->next;
    free(e);
    e = next;
  }
}

void free_env(env_t *env) 
{
  int i;
  for(i = 0; i < ENV_BUCKETS; i++) {
    free_entry(env->vars[i]);
    free_entry(env->funs[i]);
  }
}
