bin_PROGRAMS = punky

punky_SOURCES = \
        alloc.c \
        alloc.h \
        env.c   \
        env.h   \
        eval.c  \
        main.c  \
        parse.y \
        print.c \
        punky.h \
        scan.l  \
        types.h

BUILT_SOURCES = parse.h

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS = test/atoms.test test/builtins.test
MEMCHECKS = test/valgrind.atoms.test test/valgrind.builtins.test

# uncomment for memory checks (SLOW)
check_SCRIPTS += $(MEMCHECKS)

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.in test/builtins.in
TESTS_OUTPUTS = test/atoms.expect test/builtins.expect

CLEANFILES = test/*.out

EXTRA_DIST = $(check_SCRIPTS) $(TESTS_INPUTS) $(TESTS_OUTPUTS)

dist_doc_DATA = LICENSE README.md TODO.md
