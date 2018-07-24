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

ifneq (,$(wildcard ./femto-config.mk))
    include femto-config.mk
endif

BINDIR ?= bin

SUBDIRS_ADD ?=

ifeq ($(use_libcu8),1)
	SUBDIRS_ADD = libcu8
endif

SUBDIRS = libDos9 libinteval libmatheval $(SUBDIRS_ADD) dos9 dos9ize dump tea scripts po
TEAFILES = README.tea WHATSNEW.tea GUIDELINES.tea THANKS.tea
TEXTFILES = $(TEAFILES:.tea=.txt)
MDFILES = $(TEAFILES:.tea=.md)

SUBDIRS_CLEAN := $(addsuffix .clean,$(SUBDIRS))
SUBDIRS_BIN := $(addsuffix .bin,$(SUBDIRS))

HOST = $(shell $(CC) -dumpmachine)
YEAR = $(shell date +%Y)

PACKAGE = dos9
PACKAGE_URL = http://dos9.org
PACKAGE_BUGREPORT = darkbatcher@dos9.org
VERSION = 218.2
			
all: $(SUBDIRS) $(MDFILES)

$(SUBDIRS):
	$(MAKE) -C $@

clean: $(SUBDIRS_CLEAN)
	rm -f $(TEXTFILES)
	rm -rf $(BINDIR)

$(SUBDIRS_CLEAN):
	$(MAKE) -C $(basename $@) clean
	
bin: bindir $(SUBDIRS_BIN)

dist: bin
	tar zcf dos9-$(VERSION).tar.gz $(BINDIR)

git-dist: bin
	tar zcf dos9-$(VERSION)-`git rev-parse --short HEAD`.tar.gz $(BINDIR)

bindir: $(TEXTFILES)
	mkdir -p $(BINDIR)
	mkdir -p $(BINDIR)/cmd
	cp $(TEXTFILES) $(BINDIR)/
	cp COPYING* $(BINDIR)/

textfiles: $(TEXTFILES)

.tpl.tea:
	cat $< | sed -e s,\{doc[^}]*\|,\{,g > $@
	

# TEXT files are only generated when building bin or textfiles
.tea.txt:
	tea/tea -e:utf-8 -o:text-plain $< $@

# MD files are always generated when building
.tea.md:
	tea/tea -e:utf-8 -o:md $< $@
	
$(SUBDIRS_BIN): $(SUBDIRS)
	$(MAKE) -C $(basename $@) bin || true
	
# stuff to check
PROGRAMS = mimeopen xdg-open
FUNCTIONS = WIN32
LIBS = iconv intl pthread m
OPTIONS = libcu8 nls cmdlycorrect console
DEFAULTOPTIONS = no-libcu8 use-nls no-cmdlycorrect use-console
SUBCONFIG = libcu8 libmatheval
ADDITIONALVARS = HOST BINDIR YEAR VERSION PACKAGE PACKAGE_URL PACKAGE_BUGREPORT

include femto.mk

.PHONY: all bin bindir clean $(SUBDIRS) $(SUBDIRS_CLEAN) textfiles dist
.SUFFIXES: .tea .txt .md .tpl
