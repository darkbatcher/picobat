AC_INIT([Dos9],
	[2014.0.8],
	[darkbatcher@dos9.org],
	[Dos9],
	[http://www.dos9.org/])

AC_PREREQ([2.59])

AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX

AC_ARG_ENABLE([console],
	    [AC_HELP_STRING([--enable-console],
			    [Use extended console features such as color (default is yes)])],
	    [],
	    [enable_console=yes])

AS_IF([test "x$enable_console" = xno], 
      [AC_SUBST([CONSOLE], [-DLIBDOS9_NO_CONSOLE])],
      [test "x$enable_console" != xno],
      [AC_SUBST([CONSOLE], [])])

AC_CHECK_LIB([pthread],
             [pthread_create],
	     [],
             [])
AC_CHECK_LIB([intl],
             [gettext],
             [],
             [])

AC_CHECK_LIB([iconv],
             [iconv],
             [],
             [])
	
AC_CHECK_LIB([m], 
             [cos],
             [],
             [AC_MSG_ERROR("Dos9 requires math.h to be installed")])

AC_CONFIG_FILES([Makefile 
                libDos9/Makefile
                libinteval/Makefile
                libmatheval-dos9/Makefile
                dos9/Makefile
                tea/Makefile
                dump/Makefile
                po/Makefile
                scripts/Makefile])

AC_OUTPUT
