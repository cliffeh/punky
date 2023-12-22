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

BUILTIN_TYPEDEF_MACRO = '@BUILTIN_TYPEDEFS@'
BUILTIN_EXTERN_MACRO = '@BUILTIN_EXTERNS@'
BUILTIN_PATTERN_MACRO = '@BUILTIN_PATTERNS@'


def print_typedefs():
    typedefs = []
    for builtin in BUILTINS:
        typedef = f'B_TYPE_{builtin[0]}'
        if builtin[1] is not None:
            typedef += f' = \'{builtin[1]}\''
        typedefs.append(typedef)

    print('  ' + ',\n  '.join(typedefs))


def print_externs():
    print('extern sexpr NIL;')  # NIL special case
    for builtin in BUILTINS:
        print(f'extern sexpr B_{builtin[0]};')


def generate_punky_h(f):
    for line in f:
        line = line.rstrip()
        if BUILTIN_TYPEDEF_MACRO in line:
            print_typedefs()
        elif BUILTIN_EXTERN_MACRO in line:
            print_externs()
        else:
            print(line)


def print_scan_patterns():
    for builtin in BUILTINS:
        print(
            f'{builtin[3]} {{ *yylval = &B_{builtin[0]}; return(BUILTIN); }}')


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
