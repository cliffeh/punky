#include "punky.h"
#include "eval.h"
#include "env.h"

extern FILE *yyin; // our scanner's input file

struct expr_t NIL = { .type=NIL_T, .eval=&eval_idem };
struct expr_t ERROR = { .type=ERROR_T, .eval=&eval_idem };
struct expr_t T = { .type=BOOL_T, .eval=&eval_idem };
struct expr_t F = { .type=BOOL_T, .eval=&eval_idem };

punky_t *init(punky_t *p)
{
  p->in  = stdin;
  p->out = stdout;
  p->err = stderr;

  p->e = 0;
  init_env(&(p->env), 0);

  p->debug = 0;
  p->eval = 1;

  p->indent = 2;
  p->pretty = 0;

  return p;
}

punky_t *cleanup(punky_t *p)
{
  fclose(p->in);
  fclose(p->out);
  fclose(p->err);

  _free_expr(&ERROR);

  if(p->e) _free_expr(p->e);

  free_env(&(p->env));

  yylex_destroy(); // not sure we need to do this
  return p;
}

expr_t *_error(char *msg)
{
  // clean up any old error messages lying around
  _free_expr(&ERROR);

  ERROR.strval = strdup(msg);
  return &ERROR;
}
