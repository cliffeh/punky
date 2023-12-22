# punky
a simple functional language interpreter

## Notes
To add a new builtin type you have to:

  1) add a tuple to BUILTINS in mkbuiltins.py
  2) add a sexpr, a case statement, and an apply function for it (builtin.c)
  2) add it to the scanner (scan.l)
