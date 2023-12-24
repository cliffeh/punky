#pragma once

/* s-expression types */
typedef enum sexpr_type
{
  S_ERR = -1,
  S_NIL = 0,
  S_BOOL,
  S_INT,
  S_STR,
  S_QUOTE,
  // S_FLOAT, // TODO
  S_IDENT,
  S_FUN,
  S_LIST,
  S_BUILTIN
} sexpr_type;

typedef enum builtin_type
{
  B_TYPE_NIL = 0,
  B_TYPE_ADD = '+',
  B_TYPE_SUB = '-',
  B_TYPE_MUL = '*',
  B_TYPE_DIV = '/',
  B_TYPE_CAR,
  B_TYPE_CDR,
  B_TYPE_CONS,
  B_TYPE_OR,
  B_TYPE_AND,
  B_TYPE_DEFINE,
  B_TYPE_LAMBDA
} builtin_type;
