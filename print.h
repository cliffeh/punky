#ifndef _PUNKY_PRINT_H
#define _PUNKY_PRINT_H 1
#include "types.h"

void print_list(FILE *out, expr_t *e);
void print_str(FILE *out, expr_t *e);
void print_int(FILE *out, expr_t *e);
void print_float(FILE *out, expr_t *e);

#endif
