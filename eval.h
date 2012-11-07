#ifndef _PUNKY_EVAL_H
#define _PUNKY_EVAL_H 1

// TODO probably a better (and less-dangerous) way to handle this
#define    NO_ARGS(l) (l == &NIL)
#define   ONE_ARGS(l) IS_LIST(l) &&  NO_ARGS(l->cdr)
#define   TWO_ARGS(l) IS_LIST(l) && ONE_ARGS(l->cdr)
#define THREE_ARGS(l) IS_LIST(l) && TWO_ARGS(l->cdr)

/* idempotent eval function */
expr_t *eval_idem(env_t *env, expr_t *e);

/* cloning eval function */
expr_t *eval_clone(env_t *env, expr_t *e);

/* variable/function definition */
expr_t *eval_op_define(env_t *env, expr_t *e);

/* anonymous functions and function calls */
expr_t *eval_op_lambda(env_t *env, expr_t *e);
expr_t *eval_function_call(env_t *env, expr_t *fn, expr_t *args);

/* lists and identifiers */
expr_t *eval_list(env_t *env, expr_t *e);
expr_t *eval_ident(env_t *env, expr_t *e);

/* arithmetic operations */
expr_t *eval_op_add(env_t *env, expr_t *e);
expr_t *eval_op_sub(env_t *env, expr_t *e);
expr_t *eval_op_mul(env_t *env, expr_t *e);
expr_t *eval_op_div(env_t *env, expr_t *e);

/* list operations */
expr_t *eval_op_car(env_t *env, expr_t *e);
expr_t *eval_op_cdr(env_t *env, expr_t *e);
expr_t *eval_op_cons(env_t *env, expr_t *e);
expr_t *eval_op_list(env_t *env, expr_t *e);
expr_t *eval_op_append(env_t *env, expr_t *e);

/* misc operations */
expr_t *eval_op_quote(env_t *env, expr_t *e);
expr_t *eval_op_let(env_t *env, expr_t *e);

/* program control */
expr_t *eval_op_if(env_t *env, expr_t *e);

/* boolean operations */
expr_t *eval_op_not(env_t *env, expr_t *e);
expr_t *eval_op_and(env_t *env, expr_t *e);
expr_t *eval_op_or(env_t *env, expr_t *e);

/* comparison operations */
expr_t *eval_op_equal(env_t *env, expr_t *e);
expr_t *eval_op_lt(env_t *env, expr_t *e);
expr_t *eval_op_gt(env_t *env, expr_t *e);
expr_t *eval_op_le(env_t *env, expr_t *e);
expr_t *eval_op_ge(env_t *env, expr_t *e);

#endif

