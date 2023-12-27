# punky
a simple functional language interpreter

## Notes
To add a new builtin type you have to:

  1) add a tuple to BUILTINS in mkbuiltins.py
  2) write the implementation of builtin_apply_{NAME} in builtin.c.in
