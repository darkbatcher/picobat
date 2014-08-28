dnl
dnl   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
dnl   Copyright (C) 2010-2014 Romain Garbi (DarkBatcher)
dnl
dnl   This program is free software: you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation, either version 3 of the License, or
dnl   (at your option) any later version.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public License
dnl   along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

AC_INIT([Dos9],
	[2014.0.9b],
	[darkbatcher@dos9.org],
	[Dos9],
	[http://www.dos9.org/])

AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST

dnl This is a trick in order to define the host variable
dnl in a decent way
$as_echo "#define HOST \"$host\"" >> confdefs.h

AS_IF([test "x" = "y"],
      [AC_DEFINE([HOST], [1], Define to your host name)])

AM_INIT_AUTOMAKE

dnl We need libtool to compile shared libraries 
LT_INIT

dnl Perform some checks on available programs.
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_LIBTOOL
AC_STDC_HEADERS

AM_GNU_GETTEXT([external])

dnl Get .libs path
AC_SUBST([DOS9_OBJDIR], [$objdir])
AC_SUBST([SHAREDEXT], [$shrext_cmds])

AC_ARG_ENABLE([static-cmdlycorrect],
	[AC_HELP_STRING([--enable-static-cmdlycorrect],
		[foo])],
	[],
	[enable_static_cmdlycorrect=no])

AS_IF([test "x$enable_static_cmdlycorrect" = x1],
	[AC_DEFINE([DOS9_STATIC_CMDLYCORRECT],
		[1],
		Define to 1 or 0 if you want to have static cmdlycorrect)]
	[test "x$enable_static_cmdlycorrect" = x0],
	[AC_DEFINE([DOS9_STATIC_CMDLYCORRECT],
		[0],
		Define to 1 or 0 if you want to have static cmdlycorrect)]
	[AS_IF([test "x$enable_static_cmdlycorrect" != xno],
		[AC_MSG_FAILURE([Bad argument with --enable-static-cmdlycorrect]
	)])])

AC_ARG_ENABLE([console],
	    [AC_HELP_STRING([--enable-console],
			    [Use extended console features such as color (default is yes)])],
	    [],
	    [enable_console=yes])

AS_IF([test "x$enable_console" = xno], 
      [AC_DEFINE([LIBDOS9_NO_CONSOLE], [1], Define to 1 if you want to enable console feature)])

dnl Perform some header checks in order to know wether headers are present 
dnl in the platform for which we build the software. Some would say it is poor
dnl programming, because we don't ensure the usability of this headers, but it
dnl is sufficient enough to determine wether libraries are installed.
AC_CHECK_HEADERS([libintl.h])

dnl Perform some checks in order to know with which library we should link.
AC_CHECK_LIB([pthread],[pthread_create],[],[])
AC_CHECK_LIB([m],[cos],[],[AC_MSG_ERROR("No math C library")])

AC_CONFIG_SUBDIRS([libmatheval])

AC_CONFIG_FILES([Makefile
		po/Makefile
		libDos9/Makefile
                libinteval/Makefile
                dos9/Makefile
                tea/Makefile
		dump/Makefile])

AC_OUTPUT
