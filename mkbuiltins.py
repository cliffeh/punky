#!/usr/bin/env python3

import sys

BUILTINS = [
    # name,    .b_type, .sval,    pattern
    ("ADD",    '+',     "+",      "\\+"),
    ("SUB",    '-',     "-",      "\\-"),
    ("MUL",    '*',     "*",      "\\*"),
    ("DIV",    '/',     "/",      "\\/"),
    ("DEFINE", None,    "define", "[dD][eE][fF][iI][nN][eE]"),
    ("LAMBDA", None,    "lambda", "[lL][aA][mM][bB][dD][aA]"),
]

BUILTIN_TYPEDEF_MACRO = '@BUILTIN_TYPEDEF@'
BUILTIN_EXTERN_MACRO = '@BUILTIN_EXTERNS@'
BUILTIN_PATTERN_MACRO = '@BUILTIN_PATTERNS@'

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

def print_scan_patterns():
    for builtin in BUILTINS:
        print(f'{builtin[3]} {{ *yylval = &B_{builtin[0]}; return(BUILTIN); }}')

def generate_scan_l(f):
    for line in f:
        line = line.rstrip()
        if line == BUILTIN_PATTERN_MACRO:
            print_scan_patterns()
        else:
            print(line)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f'usage: {sys.argv[0]} punky.h.in|scan.l.in')
        exit(1)

    if sys.argv[1] == 'punky.h.in':
        with open('punky.h.in') as f:
            generate_punky_h(f)
    elif sys.argv[1] == 'scan.l.in':
        with open('scan.l.in') as f:
            generate_scan_l(f)
