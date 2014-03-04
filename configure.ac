AC_INIT([Dos9], [2014.0.8], [darkbatcher@dos9.org], [Dos9], [http://www.dos9.org/])
AC_PREREQ([2.59])

AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX

AC_CHECK_LIB([m], [cos], [], [AC_MSG_ERROR("Dos9 requires math.h to be installed")])
AC_CHECK_LIB([intl], [gettext], [], [AC_MSG_ERROR("Dos9 requires libintl")])
AC_CHECK_HEADER([iconv.h], [], [AC_MSG_ERROR("Dos9 requires libiconv")])

AC_CONFIG_FILES([Makefile libDos9/Makefile libinteval/Makefile libmatheval-dos9/Makefile dos9/Makefile tea/Makefile])
AC_OUTPUT