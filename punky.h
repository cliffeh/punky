#pragma once

#include <stdio.h>

/* types */
enum
{
  SEXPR_ERR = -1,
  SEXPR_NIL = 0,
  SEXPR_INT,
  SEXPR_STR,
  // SEXPR_FLOAT,
  SEXPR_IDENT,
  SEXPR_LIST,
  SEXPR_BUILTIN
};

typedef struct sexpr
{
  int type;
  union
  {
    int ival;
    char *sval;
    // float fval;
    // TODO double? other data types?
  };
  struct sexpr *car, *cdr;
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

/* memory management */
sexpr *new_err (const char *fmt, ...);
sexpr *new_nil ();
sexpr *new_int (int ival);
sexpr *new_str (const char *str);
sexpr *new_ident (const char *name);
sexpr *new_list (sexpr *car, sexpr *cdr);

// TODO free_sexpr
