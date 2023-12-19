bin_PROGRAMS = punky

punky_SOURCES = main.c parse.y punky.c punky.h scan.l
   
#    main.c        \
#    parse.h       \
#    parse.y       \
#    scan.l

BUILT_SOURCES = parse.h

ACLOCAL_AMFLAGS = -I m4
AM_YFLAGS = -d -Wno-yacc -Wcounterexamples

dist_doc_DATA = LICENSE README.md TODO.md
