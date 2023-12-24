bin_PROGRAMS = punky

punky_SOURCES =     \
        alloc.c     \
        alloc.h     \
        builtin.c   \
		builtin.h   \
        constants.h \
        enum.h      \
        env.c       \
        env.h       \
        eval.c      \
        main.c      \
        parse.y     \
        print.c     \
        punky.h     \
        scan.l

BUILT_SOURCES = builtin.c builtin.h constants.h enum.h parse.h scan.l

builtin.c: builtin.c.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

builtin.h: builtin.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

constants.h: constants.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

enum.h: enum.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

scan.l: scan.l.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall -Werror
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS =         \
    test/atoms.test     \
    test/builtins.test  \
    test/functions.test
MEMCHECKS =                      \
    test/valgrind.atoms.test     \
    test/valgrind.builtins.test  \
    test/valgrind.functions.test

# uncomment for memory checks (SLOW)
check_SCRIPTS += $(MEMCHECKS)

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.lisp test/builtins.lisp test/functions.lisp
TESTS_OUTPUTS = test/atoms.expect test/builtins.expect test/functions.expect

CLEANFILES = test/*.out

EXTRA_DIST = mkbuiltins.py builtin.c.in builtin.h.in constants.h.in enum.h.in scan.l.in \
	$(check_SCRIPTS) $(TESTS_INPUTS) $(TESTS_OUTPUTS)

dist_doc_DATA = LICENSE README.md TODO.md
