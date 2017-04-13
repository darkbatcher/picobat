#
#   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
#   Copyright (C) 2010-2014 Romain Garbi
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
dos9_LDFLAGS = -no-undefined

bin_PROGRAMS = dos9

noinst_HEADERS = command/Dos9_Alias.h command/Dos9_Ask.h command/Dos9_Block.h \
command/Dos9_Call.h command/Dos9_Cd.h command/Dos9_Cls.h command/Dos9_Color.h \
command/Dos9_CommandInfo.h command/Dos9_Commands.h command/Dos9_Copy.h \
command/Dos9_Del.h command/Dos9_Dir.h command/Dos9_Echo.h command/Dos9_Exit.h \
command/Dos9_For.h command/Dos9_Goto.h command/Dos9_Help.h command/Dos9_If.h \
command/Dos9_Mkdir.h command/Dos9_Pause.h \
command/Dos9_Rem.h command/Dos9_Ren.h command/Dos9_Rmdir.h command/Dos9_Set.h \
command/Dos9_Setlocal.h command/Dos9_Shift.h command/Dos9_Title.h \
command/Dos9_Type.h core/Dos9_ExpandAlias.h core/Dos9_Args.h \
core/Dos9_ParseBlock.h core/Dos9_Context.h core/Dos9_Core.h core/Dos9_Debug.h \
core/Dos9_ExitInt.h core/Dos9_Expand.h core/Dos9_FilePath.h \
core/Dos9_Globals.h core/Dos9_Jump.h core/Dos9_Parse.h core/Dos9_Read.h \
core/Dos9_Run.h core/Dos9_ShowIntro.h core/Dos9_SplitPath.h \
core/Dos9_Stream.h core/Dos9_Var.h core/Dos9_VersionInfo.h \
core/Dos9_FdInheritance.h core/Dos9_ReadScript.h \
errors/Dos9_Errors.h lang/Dos9_ShowHelp.h lang/Dos9_Lang.h gettext.h \
command/Dos9_Find.h command/Dos9_More.h core/Dos9_Env.h core/Dos9_OsStream.h \
command/Dos9_Start.h command/Dos9_Break.h command/Dos9_IfExp.h \
./command/Dos9_Pushd.h command/Dos9_Chcp.h \
init/Dos9_Init.h

dos9_SOURCES = ./Dos9.c ./core/Dos9_Args.c ./core/Dos9_Expand.c \
./core/Dos9_Globals.c ./core/Dos9_FdInheritance.c \
./core/Dos9_Jump.c ./core/Dos9_Run.c ./core/Dos9_SplitPath.c \
./core/Dos9_ShowIntro.c ./core/Dos9_ExitInt.c ./core/Dos9_Var.c \
./core/Dos9_Parse.c ./core/Dos9_Read.c ./core/Dos9_Stream.c \
./errors/Dos9_Errors.c ./lang/Dos9_Lang.c ./core/Dos9_Debug.c \
./core/Dos9_FilePath.c ./core/Dos9_ParseBlock.c ./lang/Dos9_ShowHelp.c \
./core/Dos9_ExpandAlias.c ./core/Dos9_ReadScript.c \
./command/Dos9_Alias.c ./command/Dos9_Ask.c \
./command/Dos9_Block.c ./command/Dos9_Call.c ./command/Dos9_Cd.c \
./command/Dos9_Cls.c ./command/Dos9_Color.c ./command/Dos9_Copy.c \
./command/Dos9_Del.c ./command/Dos9_Dir.c ./command/Dos9_Echo.c \
./command/Dos9_Exit.c ./command/Dos9_For.c ./command/Dos9_Goto.c \
./command/Dos9_Help.c ./command/Dos9_If.c ./command/Dos9_Mkdir.c \
./command/Dos9_Pause.c ./command/Dos9_Rem.c \
./command/Dos9_Ren.c ./command/Dos9_Rmdir.c ./command/Dos9_Set.c \
./command/Dos9_Setlocal.c ./command/Dos9_Shift.c ./command/Dos9_Title.c \
./command/Dos9_Type.c ./command/Dos9_Find.c ./core/Dos9_Env.c \
./command/Dos9_More.c ./core/Dos9_OsStream.c ./command/Dos9_Start.c \
./command/Dos9_Break.c ./command/Dos9_IfExp.c ./command/Dos9_Chcp.c \
./command/Dos9_Pushd.c ./init/Dos9_Init.c

dos9_LDADD = $(top_builddir)/libinteval/libinteval.la \
$(top_builddir)/libmatheval/lib/libmatheval.la $(top_builddir)/libDos9/libDos9.la \
@libcu8_lib@

AM_CFLAGS = -I$(top_srcdir)/libinteval -I$(top_srcdir)/libmatheval/lib \
-I$(top_srcdir)/libDos9 @libcu8_inc@ @CFLAGS@
