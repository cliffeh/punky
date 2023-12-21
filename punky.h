#pragma once

#include <stdio.h>

#include "env.h"
#include "types.h"

sexpr *sexpr_eval (environment *env, sexpr *e);
void sexpr_print (FILE *out, int flags, sexpr *value);
