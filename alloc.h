#pragma once

#include "punky.h"

sexpr *new_err (const char *fmt, ...);
sexpr *new_int (int ival);
sexpr *new_str (const char *str);
sexpr *new_quote (sexpr *q);
sexpr *new_ident (const char *name);
sexpr *new_pair (sexpr *car, sexpr *cdr);
sexpr *new_list (sexpr *car, sexpr *cdr);
sexpr *new_builtin (builtin_type b_type, const char *desc);

// TODO free!
