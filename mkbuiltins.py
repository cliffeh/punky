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

BUILTIN_TYPEDEFS_MACRO = '@BUILTIN_TYPEDEFS@'
BUILTIN_EXTERN_MACRO = '@BUILTIN_EXTERNS@'
BUILTIN_PATTERN_MACRO = '@BUILTIN_PATTERNS@'
BUILTIN_SINGLETON_MACRO = '@BUILTIN_SINGLETONS@'

def print_typedefs():
    typedefs = []
    for builtin in BUILTINS:
        typedef = f'B_TYPE_{builtin[0]}'
        if builtin[1] is not None:
            typedef += f' = \'{builtin[1]}\''
        typedefs.append(typedef)

    print('  ' + ',\n  '.join(typedefs))


def print_externs():
    for builtin in BUILTINS:
        print(f'extern sexpr B_{builtin[0]};')


def print_scanner_patterns():
    for builtin in BUILTINS:
        print(
            f'{builtin[3]} {{ *yylval = &B_{builtin[0]}; return(BUILTIN); }}')

def print_singletons():
    for builtin in BUILTINS:
        b_type = f"'{builtin[1]}'" if builtin[1] else f'B_TYPE_{builtin[0]}'
        print(f'sexpr B_{builtin[0]} = {{ .s_type = S_BUILTIN, .b_type = {b_type}, .sval = "{builtin[2]}" }};')

def replace_macros():
    for line in sys.stdin:
        line = line.rstrip()
        if BUILTIN_TYPEDEFS_MACRO in line:
            print_typedefs()
        elif BUILTIN_EXTERN_MACRO in line:
            print_externs()
        elif BUILTIN_PATTERN_MACRO in line:
            print_scanner_patterns()
        elif BUILTIN_SINGLETON_MACRO in line:
            print_singletons()
        else:
            print(line)


if __name__ == '__main__':
    replace_macros()
