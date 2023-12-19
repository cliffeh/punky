#pragma once

#include <stdio.h>

/* types */
enum
{
  S_NIL = 0,
  S_INT,
  S_STR,
  // SEXPR_FLOAT,
  S_IDENT,
  S_LIST
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

typedef struct environment
{
  struct env *parent;
} environment;

// TODO env!
sexpr *eval (sexpr *e, environment *env);
void print (FILE *out, sexpr *e);
