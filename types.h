#ifndef _PUNKY_TYPES_H
#define _PUNKY_TYPES_H 1

#define IS_LIST(l)   (l->type == LIST_T)
#define IS_BOOL(l)   (l->type & BOOL_T)
#define IS_INT(l)    (l->type == INT_T)
#define IS_FLOAT(l)  (l->type == FLOAT_T)
#define IS_STRING(l) (l->type == STRING_T)
#define IS_IDENT(l)  (l->type == IDENT_T)
#define IS_OP(l)     (l->type == OP_T)

/* types */
#define LIST_T       (1<<0)
#define INT_T        (1<<1)
#define FLOAT_T      (1<<2)
#define STRING_T     (1<<3)
#define IDENT_T      (1<<4)
#define OP_T         (1<<5)

/* special types */
#define NIL_T        (1<<6)
#define EOF_T        (1<<7)
#define BOOL_TRUE_T  (1<<8)
#define BOOL_FALSE_T (1<<9)

#define NUM_T        (INT_T|FLOAT_T)
#define BOOL_T       (BOOL_TRUE_T|BOOL_FALSE_T)
#define ANY_T        (LIST_T|BOOL_T|INT_T|FLOAT_T|STRING_T|IDENT_T|OP_T|NIL_T)

/* environment */
#define ENV_BUCKETS 50

typedef struct entry_t
{
  struct expr_t *id;
  struct expr_t *e;
  struct entry_t *next; // linked list
} entry_t;

typedef struct env_t
{
  struct env_t *parent;
  entry_t *entries[ENV_BUCKETS];
} env_t;

typedef struct expr_t
{
  int type;
  struct expr_t * (*eval)(struct env_t *, const struct expr_t *);
  void (*print)(FILE *out, struct expr_t *e);

  union 
  {
    /* for atoms */
    int intval;
    float floatval;
    char *strval;

    /* for lists */
    struct
    {
      struct expr_t *car;
      struct expr_t *cdr;      
    };
  };
} expr_t;

#endif
