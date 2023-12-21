# punky
a simple functional language interpreter

## Notes
To add a new builtin type you have to:

  1) add a builtin_type and an extern for it (punky.h)
  2) add a sexpr, a case statement, and an apply function for it (builtin.c)
  2) add it to the scanner (scan.l)
