#ifndef _PUNKY_H
#define _PUNKY_H 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "env.h"

/* the NIL and ERROR expressions */
extern expr_t NIL;
extern expr_t ERROR;

typedef struct punky_t
{
  /* input, output, and error streams */
  FILE *in, *out, *err;
  /* environment */
  env_t env;
  /* an expr_t that can be used to hang parse results off of */
  expr_t *e;
  /* debug flag */
  int debug;
  /* eval flag */
  int eval;
  /* printing flags */
  int indent, pretty;
} punky_t;

punky_t *init(punky_t *p);
punky_t *parse(punky_t *p);
punky_t *eval(punky_t *p);
punky_t *print(punky_t *p);
punky_t *cleanup(punky_t *p);

/* private functions for allocating expressions - use at your own risk! */
expr_t *_list_expr(expr_t *car, expr_t *cdr);
expr_t *_bool_expr(int value);
expr_t *_int_expr(int value);
expr_t *_float_expr(float value);
expr_t *_str_expr(char *value);
expr_t *_id_expr(char *value);
expr_t *_op_expr(enum PUNKY_OP_TYPE op);

/* memory management */
void _free_expr(expr_t *e);
expr_t *_clone_expr(expr_t *src);

/* error handling */
expr_t *_error(char *msg);

/* convenience function for converting enum values to strings */
char *type_to_string(enum PUNKY_TYPE t);

#endif
