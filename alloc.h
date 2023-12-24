#pragma once

#include "punky.h"

sexpr *new_err (const char *fmt, ...);
sexpr *new_int (int ival);
sexpr *new_str (const char *str);
sexpr *new_quote (sexpr *q);
sexpr *new_ident (const char *name);
sexpr *new_fun (const sexpr *params, const sexpr *body);
sexpr *new_list (const sexpr *car, const sexpr *cdr);

sexpr *sexpr_copy (const sexpr *e);
void sexpr_free (sexpr *e);
