#pragma once

#include "types.h"

typedef struct environment
{
  sexpr *entries;
  struct environment *parent;
} environment;

void env_init (environment *env, environment *parent);
sexpr *env_get (environment *env, const char *key);
void env_set (environment *env, const char *key, sexpr *e);
void env_destroy (environment *env);
