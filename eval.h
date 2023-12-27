#pragma once

#include "env.h"
#include "types.h"

sexpr *sexpr_eval (environment *env, const sexpr *e);
sexpr *sexpr_apply_builtin (environment *env, const sexpr *builtin,
                            const sexpr *args);
