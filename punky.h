#pragma once

#include <stdio.h>

/* s-expression types */
typedef enum sexpr_type
{
  S_ERR = -1,
  S_NIL = 0,
  S_INT,
  S_STR,
  S_QUOTE,
  // S_FLOAT,
  S_IDENT,
  S_PAIR,
  S_LIST,
  S_BUILTIN
} sexpr_type;

/* builtin procedure types */
typedef enum builtin_type
{
  // what should 0 be?
  B_DEFINE = 1
} builtin_type;

typedef struct sexpr
{
  sexpr_type s_type;
  union
  {
    builtin_type b_type;
    int ival;
    char *sval;
    // float fval;
    // TODO double? other data types?
    struct sexpr *car;
  };
  struct sexpr *cdr;
} sexpr;

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

sexpr *eval (environment *env, sexpr *e);
void print (FILE *out, int flags, sexpr *value);

/* environment */
sexpr *env_get (environment *env, const char *key);
sexpr *env_put (environment *env, const char *key, sexpr *e);
// TODO delete?
