#pragma once

// TODO something better than linear search through a linked list
typedef struct entry
{
  char *key;
  void *value;
  struct entry *next;
} entry;

typedef struct environment
{
  entry handle;
  struct env *parent;
} environment;

void *env_get (environment *env, const char *key);
void *env_put (environment *env, const char *key, void *e);
// TODO delete?
