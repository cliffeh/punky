AC_INIT([punky], m4_esyscmd_s([cat VERSION]), [cliff.snyder@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AM_PROG_LEX
if test "$LEX" != "flex"; then
    AC_MSG_ERROR([flex not found])
fi
AC_PROG_YACC
if test "$YACC" != "bison -y"; then
    AC_MSG_ERROR([bison not found])
fi
# TODO use something else for code generation? m4?
AM_PATH_PYTHON([3.6])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
# AC_CONFIG_SUBDIRS([popt])

AC_OUTPUT
