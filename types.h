#ifndef _PUNKY_TYPES_H
#define _PUNKY_TYPES_H 1

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
  ERROR_T
};

enum PUNKY_OP_TYPE {
  ADD_OP,
  SUB_OP,
  MUL_OP,
  DIV_OP,
  CAR_OP,
  CDR_OP,
  CONS_OP,
  LIST_OP,
  QUOTE_OP,
  DEFVAR_OP,
  DEFUN_OP,
  LET_OP,
  SUBSTR_OP,
  IF_OP,
  WHILE_OP,
  NOT_OP,
  AND_OP,
  OR_OP,
  EQUAL_OP,
  LT_OP,
  GT_OP,
  LE_OP,
  GE_OP
};

typedef struct expr_t
{
  enum PUNKY_TYPE type;
  union 
  {
    /* for primitive types (INTEGER, STRING, FLOAT, IDENT) */
    int intval;
    float floatval;
    char *strval;

    /* for built-in operations */
    enum PUNKY_OP_TYPE op;

    /* pretty much everything else is a list */
    struct
    {
      int length; // not strictly necessary, but convenient
      struct expr_t *car;
      struct expr_t *cdr;      
    };
  };
} expr_t;

#endif
