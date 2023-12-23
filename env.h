#pragma once

#include "punky.h"

// TODO something better than linear search through a linked list
typedef struct entry
{
  char *key;
  sexpr *value;
  struct entry *next;
} entry;

typedef struct environment
{
  int count;
  entry *entries;
  struct environment *parent;
} environment;

void env_init (environment *env, environment *parent);
sexpr *env_get (environment *env, const char *key);
sexpr *env_set (environment *env, const char *key, sexpr *e);
sexpr *env_del (environment *env, const char *key);
void env_destroy (environment *env);
