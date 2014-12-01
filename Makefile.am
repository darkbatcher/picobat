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

dirtree :
	mkdir -p ./bin
	mkdir -p ./bin/cmd
	mkdir -p ./bin/share

textfiles : all
	for i in README WHATSNEW; do \
		echo "- Building $$i"; \
		./tea/tea$(EXEEXT) $$i.tea $$i /e:utf-8 /o:text-plain ; \
	done;

local : textfiles dirtree
	cp ./dos9/dos9$(EXEEXT) ./bin/
	cp ./tea/tea$(EXEEXT) ./bin/cmd
	cp ./dump/dump$(EXEEXT) ./bin/cmd
	cp ./README ./bin/
	cp ./WHATSNEW ./bin/
	cp ./scripts/Dos9_Auto.bat ./bin/
	cp ./scripts/cmd/hlp.bat ./bin/cmd
	cp -R ./man ./bin/share/man
	cd po && make local

dist-bin : local
	cp -R ./bin ./$(distdir)-bin
	tardir=$(distdir)-bin && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c> $(distdir)-bin.tar.gz
	rm -R $(distdir)-bin

po-merge:
	cd po && make po-merge
