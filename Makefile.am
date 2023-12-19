bin_PROGRAMS = punky

punky_SOURCES = main.c parse.y punky.c punky.h scan.l
   
#    main.c        \
#    parse.h       \
#    parse.y       \
#    scan.l

BUILT_SOURCES = parse.h

ACLOCAL_AMFLAGS = -I m4
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

check_SCRIPTS = test/atoms.test

TESTS = $(check_SCRIPTS)
TESTS_ENVIRONMENT = SRCDIR=$(srcdir) BUILDDIR=$(builddir)
TESTS_INPUTS = test/atoms.in
TESTS_OUTPUTS = test/atoms.expect

CLEANFILES = test/*.out

EXTRA_DIST = $(check_SCRIPTS) $(TESTS_INPUTS) $(TESTS_OUTPUTS)

dist_doc_DATA = LICENSE README.md TODO.md