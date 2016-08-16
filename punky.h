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

/* boolean expressions */
extern expr_t T;
extern expr_t F;

/* a special expression for end-of-file */
extern expr_t _EOF;

typedef struct punky_t
{
  /* input, output, and error streams */
  FILE *in, *out, *err;
  /* environment */
  expr_t *env;
  /* an expr_t that can be used to hang parse results off of */
  expr_t *e;
  /* debug flag */
  int debug;
  /* eval flag */
  int eval;
  /* printing flags */
  int indent, pretty;
} punky_t;

punky_t *punky_init(punky_t *p);
punky_t *punky_read(punky_t *p);
punky_t *punky_print(punky_t *p);
punky_t *punky_cleanup(punky_t *p);

/* private functions for allocating expressions - use at your own risk! */
expr_t *_list_expr(expr_t *car, expr_t *cdr);
expr_t *_int_expr(int value);
expr_t *_float_expr(float value);
expr_t *_str_expr(char *value);
expr_t *_id_expr(char *value);
expr_t *_op_expr(char *name, expr_t * (*eval)(struct expr_t *, const struct expr_t *));
expr_t *_fun_expr(expr_t *formals, expr_t *body);
expr_t *_port_expr(FILE *fp);
expr_t *_env_expr(expr_t *values, expr_t *parent);

/* error handling */
expr_t *_err_expr(expr_t *cdr, const char *msg, const char *opt);

/* memory management */
void _free_expr(expr_t *e);
expr_t *_clone_expr(const expr_t *e);

/* expr_t comparison */
int compare(expr_t *e1, expr_t *e2);

/* convenience function for converting enum values to strings */
char *type_to_string(int t);

#endif
