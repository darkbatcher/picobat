#
#   Dos9 Makefiles, The Dos9 project
#   Copyright (C) 2012-2018  Romain Garbi, Teddy Astie
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

ROOTDIR = ..
include ../config.mk

cmds = hlp.bat
root_cmds = Dos9_Auto.bat

all:

clean:

bin: hlp_man
	mkdir -p $(BINDIR)/cmd
	cp -f $(root_cmds) $(BINDIR)
	cp -f $(cmds) $(BINDIR)/cmd

hlp_man:
	mkdir -p $(BINDIR)/share/dos9/
	cp -rL $(ROOTDIR)/man $(BINDIR)/share/dos9/hlp

.PHONY: all clean bin
