dnl Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
dnl 2012, 2013 Free Software Foundation, Inc.
dnl Copyright (C) 2014 Romain Garbi (DarkBatcher)
dnl
dnl This file is part of libmatheval
dnl
dnl GNU libmatheval is free software: you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License as
dnl published by the Free Software Foundation, either version 3 of the
dnl License, or (at your option) any later version.
dnl
dnl GNU libmatheval is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of 
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with GNU libmatheval.  If not, see
dnl <http://www.gnu.org/licenses/>.

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)
AC_INIT([libmatheval],[2014.1.0],[darkbatcher@dos9.org])
AC_CONFIG_AUX_DIR([../config])
AC_CONFIG_MACRO_DIR([../m4])
AC_SUBST([ac_aux_dir])
AM_INIT_AUTOMAKE([libmatheval], [2014.1.0])
AC_CONFIG_SRCDIR([configure.ac])
AM_CONFIG_HEADER([config.h])

LT_INIT

dnl Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_INSTALL
AM_PROG_LEX
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_YACC
AM_MISSING_PROG([AUTOM4TE], [autom4te])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([math.h strings.h stddef.h stdint.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS([bzero memset], [break])

AC_CONFIG_FILES([Makefile doc/Makefile lib/Makefile])
AC_OUTPUT(libmatheval.pc)
