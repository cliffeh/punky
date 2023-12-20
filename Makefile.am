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
        scan.l

BUILT_SOURCES = parse.h

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS = test/atoms.test
MEMCHECKS = test/valgrind-atoms.test

# uncomment for memory checks (SLOW)
check_SCRIPTS += $(MEMCHECKS)

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.in
TESTS_OUTPUTS = test/atoms.expect

CLEANFILES = test/*.out test/*.valgrind

EXTRA_DIST = $(check_SCRIPTS) $(TESTS_INPUTS) $(TESTS_OUTPUTS)

dist_doc_DATA = LICENSE README.md TODO.md
