#pragma once

/* types */
enum
{
  SEXPR_NIL = 0,
  SEXPR_INT,
  SEXPR_STR,
  // SEXPR_FLOAT,
  SEXPR_IDENT,
  SEXPR_LIST
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
