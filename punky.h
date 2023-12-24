#pragma once

#include <stdio.h> // for FILE *

#include "enum.h"

typedef struct sexpr
{
  sexpr_type s_type;
  builtin_type b_type;
  union
  {
    int ival;
    char *sval;
    // float fval; // TODO double? other data types?
    struct sexpr *car;
  };
  struct sexpr *cdr;
} sexpr;

typedef struct environment environment; // env.h

sexpr *sexpr_eval (environment *env, const sexpr *e);
void sexpr_print (FILE *out, int flags, const sexpr *value);
