dnl Copyright (c) 2014, 2015 Romain GARBI
dnl 
dnl All rights reserved.
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions are met:
dnl 
dnl * Redistributions of source code must retain the above copyright
dnl   notice, this list of conditions and the following disclaimer.
dnl * Redistributions in binary form must reproduce the above copyright
dnl   notice, this list of conditions and the following disclaimer in the
dnl   documentation and/or other materials provided with the distribution.
dnl * Neither the name of the name of Romain Garbi nor the
dnl   names of its contributors may be used to endorse or promote products
dnl   derived from this software without specific prior written permission.
dnl 
dnl THIS SOFTWARE IS PROVIDED BY ROMAIN GARBI AND CONTRIBUTORS ``AS IS'' AND ANY
dnl EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
dnl WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
dnl DISCLAIMED. IN NO EVENT SHALL ROMAIN GARBI AND CONTRIBUTORS BE LIABLE FOR ANY
dnl DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
dnl (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
dnl LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
dnl ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
dnl (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
dnl SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AC_INIT([libcu8],
	[1],
	[darkbatcher@dos9.org],
	[libcu8],
	[http://www.dos9.org/])

AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

dnl We need libtool to compile shared libraries 
LT_INIT([win32-dll])

dnl Perform some checks on available programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_STDC_HEADERS

dnl check that we have iconv
AC_CHECK_HEADERS([iconv.h])

dnl check whether iconv is a separate lib
AC_CHECK_LIB([iconv],[iconv],[],[])
AC_CHECK_LIB([iconv],[libiconv],[],[])

AC_CONFIG_FILES([Makefile
				src/Makefile
				test/Makefile])

AC_OUTPUT
