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
  B_DEFINE
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

/* memory management */
sexpr *new_err (const char *fmt, ...);
sexpr *new_nil ();
sexpr *new_int (int ival);
sexpr *new_str (const char *str);
sexpr *new_quote (sexpr *q);
sexpr *new_ident (const char *name);
sexpr *new_pair (sexpr *car, sexpr *cdr);
sexpr *new_list (sexpr *car, sexpr *cdr);

// TODO free_sexpr
