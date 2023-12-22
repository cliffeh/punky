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
  B_TYPE_NIL = 0,
  B_TYPE_DEFINE,
  B_TYPE_ADD = '+',
  B_TYPE_SUB = '-',
  B_TYPE_MUL = '*',
  B_TYPE_DIV = '/'
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

typedef struct environment environment; // env.h

sexpr *sexpr_eval (environment *env, const sexpr *e);
void sexpr_print (FILE *out, int flags, const sexpr *value);

/* singletons (in builtin.c) */
extern sexpr NIL;
extern sexpr B_DEFINE;
extern sexpr B_ADD;
extern sexpr B_SUB;
extern sexpr B_MUL;
extern sexpr B_DIV;
