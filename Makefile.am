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


EXTRA_DIST = po man scripts COPYING README WHATSNEW BUILD.readme
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects
SUBDIRS = libinteval libmatheval @libcu8@ libDos9 dos9 tea dump po dos9ize \
scripts
SHAREDEXT = @SHAREDEXT@
distbdir =$(distdir)-bin
rm_distbdir =rm -R $(distbdir)
datarootdir = @datarootdir@
datadir = @datadir@
bindir = @bindir@
install: install-exec install-data install-extra

install-extra: textfiles
	mkdir -p $(datadir)/dos9
	mkdir -p $(datadir)/dos9/hlp
	cp -Rf man/* $(datadir)/dos9/hlp
	cp README WHATSNEW $(datadir)/dos9/

.PHONY: install-extra textfiles

textfiles :
	for i in README WHATSNEW; do \
		echo "- Building $$i"; \
		case `uname -a` in \
			*MINGW*Msys*) ./tea/tea$(EXEEXT) //e:utf-8 //o:text-plain $$i.tea $$i ;; \
			*) ./tea/tea$(EXEEXT) /e:utf-8 /o:text-plain $$i.tea $$i ;;\
		esac;\
	done;

dos9-install-portable:
	-for i in libDos9 libmatheval libinteval libcu8; do \
		[ -f $(prefix)/$$i.a ] && rm -f $(prefix)/$$i.a ;\
		[ -f $(prefix)/$$i.la ] && rm -f $(prefix)/$$i.la ;\
	done;
	-[ -d $(prefix)/include ] && rm -R $(prefix)/include
	-[ -d $(prefix)/pkgconfig ] && rm -R $(prefix)/pkgconfig
	cp README $(prefix)/
	cp WHATSNEW $(prefix)/
	cp COPYING $(prefix)/
	cp THANKS $(prefix)/

dist-bin : dist-bin-dir
	tardir=$(distbdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c> $(distbdir).tar.gz
	$(rm_distbdir)

dist-bin-zip: dist-bin-dir
	-rm -f $(distbdir).zip
	zip -rq9 $(distbdir).zip $(distbdir)
	$(rm_distbdir)

po-merge:
	cd po && make po-merge
