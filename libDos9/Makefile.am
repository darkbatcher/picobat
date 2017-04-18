#
#   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
#   Copyright (C) 2010-2016 Romain Garbi (DarkBatcher)
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

AUTOMAKE_OPTIONS = foreign subdir-objects

lib_LTLIBRARIES = libDos9.la

include_HEADERS = libDos9.h 
noinst_HEADERS = libDos9-int.h ./file/Dos9_File.h

libDos9_la_SOURCES = ./cmdlist/Dos9_CmdList.c ./console/Dos9_ConsoleANSI.c \
./console/Dos9_ConsoleWin.c ./dir/Dos9_Attr.c ./dir/Dos9_Dir.c \
./dir/Dos9_UnicodeDir.c \
./enc/Dos9_Enc.c ./estr/Dos9_Estr.c stack/Dos9_Stack.c \
./thread/Dos9_Thread.c ./file/Dos9_StdFile.c \
./file/Dos9_Win32File.c ./init/Dos9_Init.c ./cmdlib/Dos9_String.c 

libDos9_la_LDFLAGS = -no-undefined
libDos9_la_LIBADD = @libcu8_lib@ @LIBS@ 

AM_CFLAGS = @libcu8_inc@ @CFLAGS@
