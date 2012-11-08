#ifndef _PUNKY_ENV_H
#define _PUNKY_ENV_H 1
#include "types.h"

void init_env(env_t *env, env_t *parent);
void put(env_t *env, expr_t *id, expr_t *e);
expr_t *get(env_t *env, expr_t *id);
void free_env(env_t *env);

#endif
