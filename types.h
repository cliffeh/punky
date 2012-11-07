#ifndef _PUNKY_TYPES_H
#define _PUNKY_TYPES_H 1

#define IS_LIST(l)   (l->type == LIST_T)
#define IS_BOOL(l)   (l->type == BOOL_T)
#define IS_INT(l)    (l->type == INTEGER_T)
#define IS_FLOAT(l)  (l->type == FLOAT_T)
#define IS_STRING(l) (l->type == STRING_T)
#define IS_IDENT(l)  (l->type == IDENTIFIER_T)
#define IS_OP(l)     (l->type == OP_T)


enum PUNKY_TYPE {
  LIST_T,
  BOOL_T,
  INTEGER_T,
  FLOAT_T,
  STRING_T,
  IDENTIFIER_T,
  OP_T,

  /* special types */
  NIL_T,
  EOF_T
};

/* environment */
#define ENV_BUCKETS 50

typedef struct entry_t
{
  char *id;
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
  enum PUNKY_TYPE type;
  struct expr_t * (*eval)(struct env_t *, struct expr_t *); // pointer to eval function
  void (*print)(FILE *out, struct expr_t *e); // pointer to print
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
