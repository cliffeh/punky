#pragma once

#include <stdio.h>

#include "env.h"
#include "types.h"

sexpr *sexpr_eval (environment *env, const sexpr *e);
void sexpr_print (FILE *out, int flags, const sexpr *value);
