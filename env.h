#ifndef _PUNKY_ENV_H
#define _PUNKY_ENV_H 1
#include "types.h"

expr_t *new_env(expr_t *parent);
void put(expr_t *env, const char *id, const expr_t *e);
expr_t *get(expr_t *env, const char *id);
void free_env(expr_t *env);

#endif
