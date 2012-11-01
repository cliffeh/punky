#ifndef _PUNKY_ENV_H
#define _PUNKY_ENV_H 1
#include "types.h"

/* environment */
#define ENV_BUCKETS 50

typedef struct entry_t
{
  char *id;
  expr_t *e;
  struct entry_t *next; // linked list
} entry_t;

typedef struct env_t
{
  struct env_t *parent;
  entry_t *funs[ENV_BUCKETS];
  entry_t *vars[ENV_BUCKETS];
} env_t;

void init_env(env_t *env, env_t *parent);
void *put_fun(env_t *env, char *id, expr_t *fundef);
void *put_var(env_t *env, char *id, expr_t *vardef);
expr_t *get_fun(env_t *env, char *id);
expr_t *get_var(env_t *env, char *id);
void free_env(env_t *env);

#endif
