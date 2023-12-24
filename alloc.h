#pragma once

#include "punky.h"

sexpr *new_err (sexpr *stack, const char *fmt, ...);
sexpr *new_int (int ival);
sexpr *new_str (const char *str);
sexpr *new_quote (sexpr *q);
sexpr *new_ident (const char *name);
sexpr *new_fun (sexpr *params, sexpr *body);
sexpr *new_list (sexpr *car, sexpr *cdr);

sexpr *sexpr_copy (const sexpr *e);
void sexpr_free (sexpr *e);
