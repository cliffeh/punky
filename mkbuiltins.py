#!/usr/bin/env python3

import sys

BUILTINS = [
    # name,    .b_type, .sval,    pattern
    # arithmetic
    ("ADD",    '+',     "+",      "\\+"),
    ("SUB",    '-',     "-",      "\\-"),
    ("MUL",    '*',     "*",      "\\*"),
    ("DIV",    '/',     "/",      "\\/"),
    # lists
    ("CAR",    None,    "car",    "[cC][aA][rR]"),
    ("CDR",    None,    "cdr",    "[cC][dD][rR]"),
    ("CONS",   None,    "cons",   "[cC][oO][nN][sS]"),
    # boolean
    ("OR",     None,    "or",     "[oO][rR]"),
    ("AND",    None,    "and",    "[aA][nN][dD]"),
    # special
    ("DEFINE", None,    "define", "[dD][eE][fF][iI][nN][eE]"),
    ("LAMBDA", None,    "lambda", "[lL][aA][mM][bB][dD][aA]"),
]

BUILTIN_TYPEDEFS_MACRO = '@BUILTIN_TYPEDEFS@'
BUILTIN_EXTERN_MACRO = '@BUILTIN_EXTERNS@'
BUILTIN_PATTERN_MACRO = '@BUILTIN_PATTERNS@'
BUILTIN_SINGLETON_MACRO = '@BUILTIN_SINGLETONS@'
BUILTIN_CASES_MACRO = '@BUILTIN_CASES@'
BUILTIN_DECLS_MACRO = '@BUILTIN_DECLS@'


def print_typedefs():  # BUILTIN_TYPEDEFS_MACRO
    typedefs = []
    for builtin in BUILTINS:
        typedef = f'B_TYPE_{builtin[0]}'
        if builtin[1] is not None:
            typedef += f' = \'{builtin[1]}\''
        typedefs.append(typedef)

    print('  ' + ',\n  '.join(typedefs))


def print_externs():  # BUILTIN_EXTERN_MACRO
    for builtin in BUILTINS:
        print(f'extern const sexpr B_{builtin[0]};')


def print_scanner_patterns():  # BUILTIN_PATTERN_MACRO
    for builtin in BUILTINS:
        print(
            f'{builtin[3]} {{ *yylval = (sexpr *)&B_{builtin[0]}; return(BUILTIN); }}')


def print_singletons():  # BUILTIN_SINGLETON_MACRO
    for builtin in BUILTINS:
        b_type = f"'{builtin[1]}'" if builtin[1] else f'B_TYPE_{builtin[0]}'
        print(
            f'const sexpr B_{builtin[0]} = {{ .s_type = S_BUILTIN, .b_type = {b_type}, .sval = "{builtin[2]}" }};')


def print_builtin_cases():  # BUILTIN_CASES_MACRO
    for builtin in BUILTINS:
        print(f'    case B_TYPE_{builtin[0]}:')
        print(f'      return builtin_apply_{builtin[0]} (env, args);')


def print_builtin_decls():  # BUILTIN_DECLS_MACRO
    for builtin in BUILTINS:
        print(
            f'static sexpr *builtin_apply_{builtin[0]} (environment *env, const sexpr *args);')


def replace_macros(filename):
    lineno = 0
    with open(filename) as f:
        for line in f:
            lineno += 1
            line = line.rstrip()
            if BUILTIN_TYPEDEFS_MACRO in line:
                # print(f'#line {lineno} "{filename}"')
                print_typedefs()
            elif BUILTIN_EXTERN_MACRO in line:
                # print(f'#line {lineno} "{filename}"')
                print_externs()
            elif BUILTIN_PATTERN_MACRO in line:
                # print(f'#line {lineno} "{filename}"') # don't want line numbers in lexer
                print_scanner_patterns()
            elif BUILTIN_SINGLETON_MACRO in line:
                # print(f'#line {lineno} "{filename}"')
                print_singletons()
            elif BUILTIN_CASES_MACRO in line:
                # print(f'#line {lineno} "{filename}"')
                print_builtin_cases()
            elif BUILTIN_DECLS_MACRO in line:
                # print(f'#line {lineno} "{filename}"')
                print_builtin_decls()
            else:
                print(line)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f'usage: {sys.argv[0]} FILE', file=sys.stderr)
        exit(1)

    replace_macros(sys.argv[1])
