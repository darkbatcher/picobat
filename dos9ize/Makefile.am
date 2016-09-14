#
#   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
#   Copyright (C) 2010-2014 Romain Garbi (DarkBatcher)
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

bin_PROGRAMS = dos9ize
dos9ize_SOURCES = dos9ize.c pairs.c
noinst_HEADERS = dos9ize.h

dos9ize_LDFLAGS = -static -no-undefined
dos9ize_LDADD = $(top_builddir)/libDos9/libDos9.la @LIBS@

AM_CFLAGS = -I$(top_srcdir)/libDos9 @CFLAGS@
