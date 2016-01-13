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

EXTRA_DIST = po man scripts COPYING README WHATSNEW
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects
SUBDIRS = libinteval libmatheval libDos9 @libcu8@ dos9 tea dump po dos9ize
SHAREDEXT = @SHAREDEXT@
distbdir =$(distdir)-bin
rm_distbdir =rm -R $(distbdir)

# a rule to make local distributions of dos9

dirtree :
	for i in ./bin ./bin/cmd ./bin/share ./bin/share/hlp; do \
		$(MKDIR_P) $$i;\
	done;

textfiles : all
	for i in README WHATSNEW; do \
		echo "- Building $$i"; \
		./tea/tea$(EXEEXT) //e:utf-8 //o:text-plain $$i.tea $$i ; \
	done;

bin : textfiles dirtree
	cp ./dos9/dos9$(EXEEXT) ./bin/
	cp ./tea/tea$(EXEEXT) ./bin/cmd
	cp ./dump/dump$(EXEEXT) ./bin/cmd
	cp ./dos9ize/dos9ize$(EXEEXT) ./bin/cmd
	cp ./README ./bin/
	cp ./WHATSNEW ./bin/
	cp ./COPYING ./bin/
	cp ./scripts/Dos9_Auto.bat ./bin/
	cp ./scripts/cmd/hlp.bat ./bin/cmd
	cp -R ./man ./bin/share/hlp
	cd po && make local

bin-strip: bin
	-for i in `find ./bin -name "*$(EXEEXT)"` ; do \
		$(STRIP) $$i;\
	done;

dist-bin-dir: bin-strip
	cp -R ./bin ./$(distdir)-bin

dist-bin : dist-bin-dir	
	tardir=$(distbdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c> $(distbdir).tar.gz
	$(rm_distbdir)

dist-bin-zip: dist-bin-dir
	-rm -f $(distbdir).zip
	zip -rq9 $(distbdir).zip $(distbdir)
	$(rm_distbdir)

po-merge:
	cd po && make po-merge
