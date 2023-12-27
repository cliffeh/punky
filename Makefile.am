bin_PROGRAMS = punky

punky_SOURCES =     \
        alloc.c     \
        alloc.h     \
        builtin.c   \
		builtin.h   \
        constants.c \
        constants.h \
        env.c       \
        env.h       \
        eval.c      \
        eval.h      \
        main.c      \
        parse.y     \
        print.c     \
        print.h     \
        scan.l      \
        types.h

BUILT_SOURCES = builtin.c builtin.h constants.c constants.h parse.h scan.l types.h

builtin.c: builtin.c.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

builtin.h: builtin.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

constants.c: constants.c.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

constants.h: constants.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

scan.l: scan.l.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

types.h: types.h.in mkbuiltins.py
	$(PYTHON) mkbuiltins.py $< > $@

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -Wall -Werror
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS =         \
    test/atoms.test     \
    test/builtins.test  \
    test/functions.test \
    test/sort.test
MEMCHECKS =                      \
    test/valgrind.atoms.test     \
    test/valgrind.builtins.test  \
    test/valgrind.functions.test \
    test/valgrind.sort.test

# uncomment for memory checks (SLOW)
check_SCRIPTS += $(MEMCHECKS)
check_PROGRAMS = test/expect

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.lisp test/builtins.lisp test/functions.lisp test/sort.lisp

EXTRA_DIST = mkbuiltins.py builtin.c.in builtin.h.in constants.c.in constants.h.in scan.l.in types.h.in \
	$(check_SCRIPTS) $(TESTS_INPUTS)

dist_doc_DATA = LICENSE README.md TODO.md
