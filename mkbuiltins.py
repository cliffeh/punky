#!/usr/bin/env python3

import sys

BUILTINS = [
    # name,    .b_type, .sval
    # ("NIL",    0,       None    ),  # special case
    ("ADD",    '+',     "+"),
    ("SUB",    '-',     "-"),
    ("MUL",    '*',     "*"),
    ("DIV",    '/',     "/"),
    ("DEFINE", None,    "define"),
    ("LAMBDA", None,    "lambda"),
]

BUILTIN_TYPEDEF_MACRO = '@BUILTIN_TYPEDEF@'
BUILTIN_EXTERN_MACRO = '@BUILTIN_EXTERNS@'

def print_typedefs():
    print('typedef enum builtin_type\n{')
    typedefs = ['  B_TYPE_NIL = 0']  # NIL special case
    for builtin in BUILTINS:
        typedef = f'B_TYPE_{builtin[0]}'
        if builtin[1] is not None:
            typedef += f' = \'{builtin[1]}\''
        typedefs.append(typedef)

    print(',\n  '.join(typedefs))
    print('} builtin_type;')

def print_externs():
    print('extern sexpr NIL;')  # NIL special case
    for builtin in BUILTINS:
        print(f'extern sexpr B_{builtin[0]};')

def generate_punky_h(f):
    for line in f:
        line = line.rstrip()
        if line == BUILTIN_TYPEDEF_MACRO:
            print_typedefs()
        elif line == BUILTIN_EXTERN_MACRO:
            print_externs()
        else:
            print(line)


def mkexterns():
    print('extern sexpr NIL;')
    for builtin in BUILTINS:
        print(f'extern sexpr B_{builtin[0]};')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f'usage: {sys.argv[0]} punky.h.in')
        exit(1)

    if sys.argv[1] == 'punky.h.in':
        with open('punky.h.in') as f:
            generate_punky_h(f)
