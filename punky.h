#pragma once

#include <stdio.h>

/* types */
enum
{
  SEXPR_NIL = 0,
  SEXPR_INT,
  SEXPR_STR,
  // SEXPR_FLOAT,
  SEXPR_LIST,
  SEXPR_IDENT
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

/* memory management */
sexpr *new_nil();
sexpr *new_int(int ival);
sexpr *new_str(char *sval);
sexpr *new_ident(char *sval);
sexpr *new_list(sexpr *car, sexpr *cdr);


// TODO free_sexpr
