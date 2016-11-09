dnl
dnl   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
dnl   Copyright (C) 2010-2014 Romain Garbi
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
	[216.1],
	[darkbatcher@dos9.org],
	[Dos9],
	[http://www.dos9.org/])

AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
dnl This is a trick in order to define the host variable
dnl in a decent way
AC_DEFINE_UNQUOTED([HOST], ["$host"], Define to your host name)

AM_INIT_AUTOMAKE

dnl We need libtool to compile shared libraries 
LT_INIT([shared,win32-dll])

dnl Perform some checks on available programs.
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_LIBTOOL
AC_STDC_HEADERS

AM_GNU_GETTEXT([external])
LIBS="$LIBS $LIBINTL"

dnl Get .libs path
AC_SUBST([DOS9_OBJDIR], [$objdir])
AC_SUBST([SHAREDEXT], [$shrext_cmds])

AC_ARG_ENABLE([static-cmdlycorrect],
	[AC_HELP_STRING([--enable-static-cmdlycorrect],
		[Enable static cmdlycorrect setlocal])],
	[],
	[enable_static_cmdlycorrect=no])

AS_IF([test "x$enable_static_cmdlycorrect" = x1],
	[AC_DEFINE([DOS9_STATIC_CMDLYCORRECT],
		[1],
		Define to 1 or 0 if you want to have static cmdlycorrect)],
	[test "x$enable_static_cmdlycorrect" = x0],
	[AC_DEFINE([DOS9_STATIC_CMDLYCORRECT],
		[0],
		Define to 1 or 0 if you want to have static cmdlycorrect)],
	[test "x$enable_static_cmdlycorrect" != xno],
		[AC_MSG_FAILURE([Bad argument with --enable-static-cmdlycorrect]
	)])

AC_ARG_ENABLE([console],
	    [AC_HELP_STRING([--enable-console],
			    [Use extended console features such as color (default is yes)])],
	    [],
	    [enable_console=yes]
	)
		
AC_ARG_ENABLE([libcu8],
		[AC_HELP_STRING([--enable-libcu8],
                 [Use libcu8 to add utf8 support using msvcrt using windows])],
		[],
		[enable_libcu8=yes]
	)

AS_IF([test "x$enable_console" = xno], 
      [AC_DEFINE([LIBDOS9_NO_CONSOLE], [1], Define to 1 if you want to enable console feature)])

dnl Perform some header checks in order to know wether headers are present 
dnl in the platform for which we build the software. Some would say it is poor
dnl programming, because we don't ensure the usability of this headers, but it
dnl is sufficient enough to determine wether libraries are installed.
AC_CHECK_HEADERS([libintl.h])

dnl check start equivalent existence
	
dnl Perform some checks in order to know with which library we should link.
case $host_os in
	*darwin*|*linux*|*bsd*)
dnl link to pthread only on *nix target
		AC_CHECK_LIB([pthread],[pthread_create],[],[])
dnl look for xdg-open
		AC_PATH_PROG([XDG_OPEN], [xdg-open])  
		AS_IF([test "x$XDG_OPEN" != x],
			[AC_DEFINE_UNQUOTED([XDG_OPEN], ["$XDG_OPEN"],
			Define to 1 if have xdg-open)]
		)
dnl No libcu8 for *nixes
		enable_libcu8=no
	;;
	
	*);;
esac

dnl hack to expand cleanly the $datadir variable
dos9data=`eval echo -e "$(eval echo -e "$datadir")"`

dnl define this for some weird unices
AC_DEFINE_UNQUOTED([BINARY_PATH], ["$bindir"], Define to the binary directory)
AC_DEFINE_UNQUOTED([DATA_PATH], ["$dos9data"], Define to the data directory)

AC_CHECK_LIB([m],[cos],[],[AC_MSG_ERROR("No math C library")])

if test "x$enable_libcu8" != xno; then
	   echo Check wether libcu8 should be enabled ... yes 
	   AC_DEFINE([DOS9_USE_LIBCU8], [1], [Define to 1 if you want to enable libcu8])
	   AC_SUBST([libcu8], [libcu8])
	   AC_SUBST([libcu8_lib], [$\(top_srcdir\)/libcu8/src/libcu8.la])
	   AC_SUBST([libcu8_inc], [-I$\(top_srcdir\)/libcu8/include])
	   AC_CONFIG_SUBDIRS([libcu8])
else 
	   echo Check wether libcu8 should be enabled ... no
       AC_SUBST([libcu8_lib],[ ])
	   AC_SUBST([libcu8_inc],[ ])
	   AC_SUBST([libcu8],[ ])

fi

AC_CONFIG_SUBDIRS([libmatheval])

AC_CONFIG_FILES([Makefile
		po/Makefile
		libDos9/Makefile
        	libinteval/Makefile
        	dos9/Makefile
        	tea/Makefile
		dump/Makefile
		dos9ize/Makefile
		scripts/Makefile])

AC_OUTPUT
