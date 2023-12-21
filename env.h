#pragma once

#include "types.h"

// TODO something better than linear search through a linked list
typedef struct entry
{
  char *key;
  sexpr *value;
  struct entry *next;
} entry;

typedef struct environment
{
  entry handle;
  struct env *parent;
} environment;

sexpr *env_get (environment *env, const char *key);
void env_set (environment *env, const char *key, sexpr *e);
// TODO delete?
