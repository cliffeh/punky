#pragma once

#include "env.h"
#include "types.h"

sexpr *sexpr_apply_inapplicable (const sexpr *self, const sexpr *args,
                                 environment *env);

sexpr *sexpr_apply_function (const sexpr *self, const sexpr *args,
                             environment *env);

sexpr *sexpr_eval_builtin (const sexpr *self, environment *env);
sexpr *sexpr_eval_copy (const sexpr *self, environment *env);
sexpr *sexpr_eval_ident (const sexpr *self, environment *env);
sexpr *sexpr_eval_function (const sexpr *self, environment *env);
sexpr *sexpr_eval_list (const sexpr *self, environment *env);
sexpr *sexpr_eval_quote (const sexpr *self, environment *env);
