#include "punky.h"

expr_t *new_env(expr_t *parent)
{
  return _env_expr(&NIL, parent);
}

static expr_t *find_entry(const expr_t *env, const char *id)
{
  // TODO something with O(log n) lookup time instead of a simple linked list
  expr_t *r;
  for(r = env->car; r != &NIL; r = r->cdr) {
    if(strcmp(id, r->car->car->strval) == 0) return r;
  }
  return r;
}

void put(expr_t *env, const char *id, const expr_t *e)
{
  expr_t *new_record = _list_expr(_id_expr(strdup(id)), _clone_expr(e));

  expr_t *entry = find_entry(env, id);
  if(entry == &NIL) { // new entry
    entry = _list_expr(new_record, env->car);
    env->car = entry;
  } else { // replacement entry
    _free_expr(entry->car);
    entry->car = new_record;
  }
}

expr_t *get(expr_t *env, const char *id)
{
  expr_t *entry = find_entry(env, id);
  if(entry != &NIL) return _clone_expr(entry->car->cdr);
  if(env->cdr != &NIL) return get(env->cdr, id);
  return &NIL;
}

expr_t *keys(const expr_t *env)
{
  expr_t *e, *r = &NIL;
  for(e = env->car; e != &NIL; e = e->cdr) {
    r = _list_expr(_clone_expr(e->car->car), r);
  }
  return r;
}

void free_env(expr_t *env) 
{
  _free_expr(env);
}
