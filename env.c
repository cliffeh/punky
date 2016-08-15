#include "punky.h"

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
    env->entries[i] = 0;
  }
  env->parent = parent;
}

static entry_t *_find_entry(env_t *env, char *id, entry_t *bucket)
{
  entry_t *entry;
  for(entry = bucket; entry; entry = entry->next) {
    if(strcmp(id, entry->id->strval) == 0) {
      return entry;
    }
  }
  // sorry, didn't find it
  return 0;
}

void put(env_t *env, const expr_t *id, const expr_t *e)
{
  if(!(IS_IDENT(id))) {
    fprintf(stderr, "put: error: invalid id expression\n");
    return;
  }

  // we need to know which bucket we're in
  int i = hash(id->strval);

  // see if an entry with this id already exists
  entry_t *entry = _find_entry(env, id->strval, env->entries[i]);

  if(entry) { 
    // we're replacing this entry, so let's free up old pointers
    _free_expr(entry->id);
    _free_expr(entry->e);
  } else {
    // create the new entry
    entry = malloc(sizeof(entry_t));

    // make sure we keep track of any already-existing entries in this bucket
    entry->next = env->entries[i];
  
    // we'll append our new entry at the head of the list
    env->entries[i] = entry;
  }
  
  entry->id = _clone_expr(id);
  entry->e = _clone_expr(e);
}

expr_t *get(env_t *env, const expr_t *id)
{
  expr_t *r = 0;
  
  if(!(IS_IDENT(id))) {
    return _err_expr(0, "get: error: invalid id expression", 0);
  } else {
    // we need to know what bucket we're in
    int i = hash(id->strval);
    entry_t *entry = _find_entry(env, id->strval, env->entries[i]);

    if(entry) {
      // we found it! let's return it...
      r = _clone_expr(entry->e);
    } else if(env->parent) {
      r = get(env->parent, id);
    }
  }

  if(!r) r = _err_expr(0, "get: unbound variable", id->strval);
  return r;
}

void free_entry(entry_t *entry) 
{
  entry_t *e = entry, *next;
  while(e) {
    _free_expr(e->id);
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
    free_entry(env->entries[i]);
  }
}
