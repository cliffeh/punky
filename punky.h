#pragma once

#include "env.h"
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
  B_DEFINE = 1,
  B_PLUS = '+'
} builtin_type;

typedef struct sexpr
{
  sexpr_type s_type;
  builtin_type b_type;
  union
  {
    int ival;
    char *sval;
    // float fval;
    // TODO double? other data types?
    struct sexpr *car;
  };
  struct sexpr *cdr;
} sexpr;

sexpr *sexpr_eval (environment *env, sexpr *e);
void sexpr_print (FILE *out, int flags, sexpr *value);
