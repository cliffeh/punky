bin_PROGRAMS = punky

punky_SOURCES =   \
        alloc.c   \
        alloc.h   \
        builtin.c \
		builtin.h \
        env.c     \
        env.h     \
        eval.c    \
        main.c    \
        parse.y   \
        print.c   \
        punky.h   \
        scan.l

BUILT_SOURCES = builtin.c builtin.h parse.h punky.h scan.l

builtin.c: builtin.c.in
	$(PYTHON) mkbuiltins.py < $< > $@

builtin.h: builtin.h.in
	$(PYTHON) mkbuiltins.py < $< > $@

punky.h: punky.h.in
	$(PYTHON) mkbuiltins.py < $< > $@

scan.l: scan.l.in
	$(PYTHON) mkbuiltins.py < $< > $@

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS = test/atoms.test test/builtins.test
MEMCHECKS = test/valgrind.atoms.test test/valgrind.builtins.test

# uncomment for memory checks (SLOW)
check_SCRIPTS += $(MEMCHECKS)

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.lisp test/builtins.lisp
TESTS_OUTPUTS = test/atoms.expect test/builtins.expect

CLEANFILES = test/*.out

EXTRA_DIST = builtin.c.in builtin.h.in punky.h.in scan.l.in \
	$(check_SCRIPTS) $(TESTS_INPUTS) $(TESTS_OUTPUTS)

dist_doc_DATA = LICENSE README.md TODO.md
