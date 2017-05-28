#ifndef _PUNKY_EVAL_H
#define _PUNKY_EVAL_H 1

// TODO probably a better (and less-dangerous) way to handle this
#define    NO_ARGS(l) (l == &NIL)
#define   ONE_ARGS(l) IS_LIST(l) &&  NO_ARGS(l->cdr)
#define   TWO_ARGS(l) IS_LIST(l) && ONE_ARGS(l->cdr)
#define THREE_ARGS(l) IS_LIST(l) && TWO_ARGS(l->cdr)

#define   _NO_ARGS(l) (l == &NIL)
#define  _ONE_ARGS(l, a1, t1) (IS_LIST(l) && ((a1 = l->car->eval(env, l->car)) & t1) && NO_ARGS(t1))
#define  _TWO_ARGS(l, a1, t1) (IS_LIST(l) && ((a1 = l->car->eval(env, l->car)) & t1) && NO_ARGS(t1))

/* clone eval function */
expr_t *eval_clone(expr_t *env, const expr_t *e);

/* cloning eval function */
expr_t *eval_clone(expr_t *env, const expr_t *e);

/* variable/function definition */
expr_t *eval_op_define(expr_t *env, const expr_t *e);

/* anonymous functions and function calls */
expr_t *eval_op_lambda(expr_t *env, const expr_t *e);
expr_t *eval_fun(expr_t *env, const expr_t *e);

/* lists and identifiers */
expr_t *eval_list(expr_t *env, const expr_t *e);
expr_t *eval_ident(expr_t *env, const expr_t *e);

/* arithmetic operations */
expr_t *eval_op_add(expr_t *env, const expr_t *e);
expr_t *eval_op_sub(expr_t *env, const expr_t *e);
expr_t *eval_op_mul(expr_t *env, const expr_t *e);
expr_t *eval_op_div(expr_t *env, const expr_t *e);

/* list operations */
expr_t *eval_op_car(expr_t *env, const expr_t *e);
expr_t *eval_op_cdr(expr_t *env, const expr_t *e);
expr_t *eval_op_cons(expr_t *env, const expr_t *e);
expr_t *eval_op_list(expr_t *env, const expr_t *e);
expr_t *eval_op_append(expr_t *env, const expr_t *e);

/* misc operations */
expr_t *eval_op_quote(expr_t *env, const expr_t *e);
expr_t *eval_op_let(expr_t *env, const expr_t *e);

/* program control */
expr_t *eval_op_if(expr_t *env, const expr_t *e);

/* boolean operations */
expr_t *eval_op_not(expr_t *env, const expr_t *e);
expr_t *eval_op_and(expr_t *env, const expr_t *e);
expr_t *eval_op_or(expr_t *env, const expr_t *e);

/* comparison operations */
expr_t *eval_op_equal(expr_t *env, const expr_t *e);
expr_t *eval_op_lt(expr_t *env, const expr_t *e);
expr_t *eval_op_gt(expr_t *env, const expr_t *e);
expr_t *eval_op_le(expr_t *env, const expr_t *e);
expr_t *eval_op_ge(expr_t *env, const expr_t *e);

/* string operations */
expr_t *eval_op_substr(expr_t *env, const expr_t *e);
expr_t *eval_op_strlen(expr_t *env, const expr_t *e);
expr_t *eval_op_split(expr_t *env, const expr_t *e);
expr_t *eval_op_concat(expr_t *env, const expr_t *e);

/* port operations */
expr_t *eval_op_openif(expr_t *env, const expr_t *e);
expr_t *eval_op_closeif(expr_t *env, const expr_t *e);
expr_t *eval_op_readline(expr_t *env, const expr_t *e);

/* env operations */
expr_t *eval_op_env(expr_t *env, const expr_t *e);
expr_t *eval_op_keys(expr_t *env, const expr_t *e);

/* misc operations */
expr_t *eval_op_time(expr_t *env, const expr_t *e);
expr_t *eval_op_sleep(expr_t *env, const expr_t *e);

#endif
