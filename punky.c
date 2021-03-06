#include "punky.h"
#include "eval.h"
#include "env.h"

extern FILE *yyin; // our scanner's input file

struct expr_t NIL =  { .type=NIL_T,        .eval=&eval_clone, .car = &NIL, .cdr = &NIL };
struct expr_t T =    { .type=BOOL_TRUE_T,  .eval=&eval_clone, .car = &NIL, .cdr = &NIL };
struct expr_t F =    { .type=BOOL_FALSE_T, .eval=&eval_clone, .car = &NIL, .cdr = &NIL };
struct expr_t _EOF = { .type=EOF_T,        .eval=&eval_clone, .car = &NIL, .cdr = &NIL };

int yylex_destroy();

punky_t *punky_init(punky_t *p)
{
  p->in  = stdin;
  p->out = stdout;
  p->err = stderr;

  p->e = &NIL;
  p->env = new_env(&NIL);

  p->debug = 0;
  p->eval = 1;

  p->indent = 2;
  p->pretty = 0;

  return p;
}

punky_t *punky_cleanup(punky_t *p)
{
  fclose(p->in);
  fclose(p->out);
  fclose(p->err);

  if(p->e) _free_expr(p->e);

  free_env(p->env);

  yylex_destroy(); // not sure we need to do this
  return p;
}
