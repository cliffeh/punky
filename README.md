# punky
a simple functional language interpreter

## Notes
To add a new builtin type you have to:

  1) add a builtin_type and an extern for it (types.h)
  2) add it to the scanner (scan.l)
  3) add a sexpr for it (alloc.c)
  4) add a case statement and apply function for it (builtin.c)
