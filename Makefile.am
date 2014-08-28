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

EXTRA_DIST = po man scripts LICENSE README WHATSNEW
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects
SUBDIRS = libinteval libmatheval libDos9 dos9 tea dump po

SHAREDEXT = @SHAREDEXT@

# a rule to make local distributions of dos9
local : all
	mkdir -p ./bin
	cp ./dos9/@DOS9_OBJDIR@/dos9$(EXEEXT) ./bin/
	cp ./libDos9/@DOS9_OBJDIR@/libDos9-*$(SHAREDEXT) ./bin/
	mkdir -p ./bin/cmd
	cp ./tea/@DOS9_OBJDIR@/tea$(EXEEXT) ./bin/cmd
	cp ./dump/@DOS9_OBJDIR@/dump$(EXEEXT) ./bin/cmd
	cd po && make local
	
po-merge:
	cd po && make po-merge
