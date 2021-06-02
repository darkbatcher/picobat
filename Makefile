#
#   pBat Makefiles, The pBat project
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

ifneq (,$(wildcard ./femto-config.mk))
	include femto-config.mk
endif

BINDIR ?= bin
SUBDIRS_ADD ?=
SUBDIR_PO =

ifeq ($(use_libcu8),1)
	SUBDIRS_ADD = libcu8
endif

ifeq ($(use_nls),1)
	SUBDIR_PO = po
endif

SUBDIRS = microgettext libpbat libmatheval $(SUBDIRS_ADD) pbat pbatize dump tea \
			scripts modules $(SUBDIR_PO)
TEAFILES = README.tea WHATSNEW.tea GUIDELINES.tea THANKS.tea
TEXTFILES = $(TEAFILES:.tea=.txt)
MDFILES = $(TEAFILES:.tea=.md)
MANFILES = man/en_US/readme.tea

SUBDIRS_CLEAN := $(addsuffix .clean,$(SUBDIRS))
SUBDIRS_BIN := $(addsuffix .bin,$(SUBDIRS))

HOST = $(shell $(CC) -dumpmachine)
YEAR = $(shell date +%Y)

PACKAGE = picobat
PACKAGE_URL = http://picobat.org
PACKAGE_BUGREPORT = darkbatcher@picobat.org
VERSION = $(shell date +%Y | sed s/0//).$(shell date +%m)

all: $(SUBDIRS) $(MDFILES)

$(SUBDIRS):
	$(MAKE) -C $@

clean: $(SUBDIRS_CLEAN)
	rm -f $(TEXTFILES)
	rm -rf $(BINDIR)

$(SUBDIRS_CLEAN):
	$(MAKE) -C $(basename $@) clean

bin: all bindir $(SUBDIRS_BIN) 

dist: bin
	mv bin picobat-$(VERSION)
	zip -r pbat-$(VERSION).zip picobat-$(VERSION)

src-dist:
	tar zcf picobat-$(VERSION)-src.tar.gz --transform 's,^,picobat-$(VERSION)/,' `git ls-files`

git-dist: bin
	tar zcf picobat-$(VERSION)-`git rev-parse --short HEAD`.tar.gz $(BINDIR)

bindir: $(MANFILES) $(TEXTFILES)
	mkdir -p $(BINDIR)
	mkdir -p $(BINDIR)/cmd
	cp $(TEXTFILES) $(BINDIR)/
	cp COPYING* $(BINDIR)/

textfiles: $(TEXTFILES)

man/en_US/readme.tea : README.tpl
	cat $< | sed -e s,\{doc/,\{,g > $@

doc.md: README.tpl
	./tea/tea$(EXEC_SUFFIX) -e:utf-8 -o:md README.tpl doc.md

.tpl.tea:
	cat $< footer.tpl | sed -e s,\{doc[^}]*\|,\{,g > $@

.tea.txt:
	./tea/tea$(EXEC_SUFFIX) -e:utf-8 -o:text-plain $< $@

.tea.md:
	./tea/tea$(EXEC_SUFFIX) -e:utf-8 -o:md $< $@

$(SUBDIRS_BIN): $(SUBDIRS)
	$(MAKE) -C $(basename $@) bin || true

# stuff to check
PROGRAMS = mimeopen xdg-open
FUNCTIONS = WIN32 MINGW_W64
LIBS = pthread m dl
FLAGS = PIC
OPTIONS = libcu8 nls cmdlycorrect console modules linenoise w10ansi
DEFAULTOPTIONS = no-libcu8 use-nls no-cmdlycorrect use-console use-modules \
				 use-linenoise no-w10ansi
SUBCONFIG = libcu8 libmatheval
ADDITIONALVARS = HOST BINDIR YEAR VERSION PACKAGE PACKAGE_URL PACKAGE_BUGREPORT

include femto.mk

.PHONY: all bin bindir clean $(SUBDIRS) $(SUBDIRS_CLEAN) textfiles dist
.SUFFIXES: .tea .txt .md .tpl
